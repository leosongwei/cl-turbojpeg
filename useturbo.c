#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <turbojpeg.h>

struct dimg{
	int width;
	int height;
	unsigned char* buffer;
	const char* err;
};

static const char FILE_NOT_OPEN[] = "File can not be opened.";

// fopen, fseek, fread, fclose
struct dimg* decompress_img(char* path)
{
	struct dimg* dimg = (struct dimg*)malloc(sizeof(struct dimg));
	int garbage;

	FILE* file_jpeg = fopen(path, "r");
	if(NULL == file_jpeg) goto failure_file;

	fseek(file_jpeg, 0, SEEK_END);
	size_t size_jpeg = ftell(file_jpeg);
	fseek(file_jpeg, 0, SEEK_SET);

	unsigned char* buf_jpeg = (unsigned char*)malloc(size_jpeg); // resource: buffer
	fread(buf_jpeg, size_jpeg, 1, file_jpeg);
	fclose(file_jpeg);

	tjhandle decompressor = tjInitDecompress(); // resource: handler
	if(NULL == decompressor) goto failure_init;

	if(0 != tjDecompressHeader3(decompressor,
				buf_jpeg, size_jpeg,
				&(dimg->width), &(dimg->height),
				&garbage, &garbage))
		goto failure_header;

	unsigned char* buf_img =
		(unsigned char*)malloc(3*(dimg->width)*(dimg->height)); // resource: another buffer
	// buffer of 3 colors: R G B R G B.....

	if(0 != tjDecompress2(decompressor,
				buf_jpeg, size_jpeg,
				buf_img,
				dimg->width, 0, dimg->height,
				TJPF_RGB,
				TJFLAG_FASTDCT))
		goto failure_decompress;

	dimg->buffer = buf_img;
	tjDestroy(decompressor);

end:
	return dimg;

failure_file:
	dimg->err = FILE_NOT_OPEN;
	goto end;

failure_decompress:
	free(buf_img);
failure_header:
	tjDestroy(decompressor);
failure_init:
	free(buf_jpeg);
	dimg->err = tjGetErrorStr(); // Caution: error report is not thread safe in libjpeg-turbo
	goto end;
}



(in-package :cl-user)

(defpackage :asd-cl-turbojpeg
  (:use :cl :asdf))

(in-package :asd-cl-turbojpeg)

(defsystem :cl-turbojpeg
  :version "0.1"
  :depends-on (:uffi)
  :serial t)

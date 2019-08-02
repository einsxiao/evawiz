;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Name: evawiz.el
;;; Version: 1.0©2015
;;; Author: Xiao,Hu 
;;; Intro: evawiz mode for emacs to corporate with platform Evawiz.
;;;		   It is derived from from the mode written for mathematica.
;;;
;;;     evawiz-command-line                    "math"
;;;     evawiz-always-start-kernel-with-mode   nil
;;;     evawiz-split-on-startup                nil
(defvar evawiz-command-line "evawiz" "How to access the command-line interface to Evawiz on your system.") 
(defvar evawiz-kernel-input-max-amount 100 "Max amount of input will be recorded")
;;; The rest of these variables are internal
;;(defvar evawiz-startup-with-kernel-if-no-args t)
(defvar evawiz-startup-with-kernel-if-no-args nil)
(defvar math 'evawiz-start-kernel)

(setq auto-mode-alist
      (append '(("\\.ev\\'" . evawiz-ev-mode))   
              auto-mode-alist))

(require 'evawiz-kernel)
(require 'evawiz-ev)
(provide 'evawiz)



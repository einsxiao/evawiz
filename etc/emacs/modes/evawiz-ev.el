;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Name: evawiz-ev.el
;;; Version: 1.0©2015
;;; Author: Xiao,Hu 
;;; Intro: evawiz-ev mode for emacs to corporate with Evawiz m files.

;;; These variables you can change

(defvar evawiz-ev-always-start-kernel-with-mode nil
  "If t, a Evawiz kernel will be started every time you enter
Evawiz Mode (either by M-x evawiz-ev-mode RET or by visiting a
.ev file)."
  )

;; added by p.weitershausen@physik.tu-dresden.de
(defvar evawiz-ev-never-start-kernel-with-mode t
  "If t, a Evawiz kernel will never be started when you enter
Evawiz Mode (either by M-x evawiz-ev-mode RET or by visiting a
.ev file)."
  )
;; end addition by p.weitershausen@physik.tu-dresden.de

;;(defvar evawiz-ev-split-on-startup t
(defvar evawiz-ev-split-on-startup nil
  "If t, entering Evawiz mode will split the screen to show you
the kernel starting up."
  )

;;; The rest of these variables are internal

(defvar evawiz-kernel-workbuf ()
  "An association list connecting Evawiz processes with working
buffers."
  )

(defvar evawiz-kernel-marks ()
  "An association list connecting Evawiz processes with the mark
queue."
  )

(defvar evawiz-ev-mode-map ()
  "Keymap used in Evawiz-EV mode."
  )
(if evawiz-ev-mode-map
    ()
  (setq evawiz-ev-mode-map (make-sparse-keymap))
  (define-key evawiz-ev-mode-map "\C-j" 'evawiz-ev-execute)
  (define-key evawiz-ev-mode-map (kbd "<f5>") 'evawiz-ev-execute-all)
  (define-key evawiz-ev-mode-map "\C-ca" 'evawiz-abort-calculation)
  (define-key evawiz-ev-mode-map "\C-cs" 'evawiz-start-split-screen)
  (define-key evawiz-ev-mode-map "\C-cr" 'evawiz-restart-kernel)
  (define-key evawiz-ev-mode-map "\C-ck" 'evawiz-kill-this-kernel)
  (define-key evawiz-ev-mode-map "\C-d" 'evawiz-kill-this-kernel)
  (define-key evawiz-ev-mode-map "\177" 'backward-delete-char-untabify)
  (define-key evawiz-ev-mode-map ")" 'evawiz-evlectric-paren)
  (define-key evawiz-ev-mode-map "]" 'evawiz-evlectric-braket)
  (define-key evawiz-ev-mode-map "}" 'evawiz-evlectric-brace) 
 )

(defun evawiz-ev-mode ()
  "Major mode for editing Evawiz plain text files (.ev) in Emacs.
Commands:
\\{evawiz-ev-mode-map}"
  (interactive)
  (kill-all-local-variables)

  (use-local-map evawiz-ev-mode-map)

  (setq major-mode 'evawiz-ev-mode)
  (setq mode-name "Evawiz-EV")

  (setq local-abbrev-table evawiz-evode-abbrev-table)
  (set-syntax-table evawiz-evode-syntax-table)

  (make-local-variable 'indent-line-function)
  (setq indent-line-function 'evawiz-indent-line)

  (make-local-variable 'comment-start)
  (setq comment-start "(*")
  (make-local-variable 'comment-end)
  (setq comment-end "*)")
  (make-local-variable 'comment-start-skip)
  (setq comment-start-skip "\\(//+\\|/\\*+\\)\\s *\\#")

  (make-local-variable 'paragraph-start)
  (setq paragraph-start (concat page-delimiter "\\|$"))
  (make-local-variable 'paragraph-separate)
  (setq paragraph-separate paragraph-start)

  (make-local-variable 'font-lock-defaults)
  (setq font-lock-defaults '(evawiz-font-lock-keywords nil t))

  (make-local-variable 'kill-buffer-hook)

  ;; (if (and
  ;;       (not evawiz-ev-never-start-kernel-with-mode)
  ;;       (or evawiz-ev-always-start-kernel-with-mode
  ;;           (y-or-n-p "Start a Evawiz kernel for evaluations? "))
  ;;       ) ; end of logical expression
  ;;   (progn
  ;;     (evawiz-internal-start-kernel (current-buffer))
  ;;     (if evawiz-ev-split-on-startup
  ;;         (if (not (get-buffer-window "*Evawiz-Kernel*"))
  ;;             (evawiz-start-split-screen)
  ;;           ) ; end if the buffer is not visible
  ;;       ) ; end if split on startup is true
  ;;     ) ; end of progn
  ;;   (message "You can start a Evawiz kernel with C-c r or M-x evawiz-restart-kernel.")
  ;;   ) ; end if

  (run-mode-hooks 'evawiz-ev-mode-hook)
  )

(defun evawiz-ev ()
  "Start a Evawiz process in a new buffer."
  (interactive)
  (let ((oldval evawiz-ev-always-start-kernel-with-mode))
    (setq evawiz-ev-always-start-kernel-with-mode t)
    (switch-to-buffer (generate-new-buffer "tmp.ev"))
    (evawiz-ev-mode)
    (setq evawiz-ev-always-start-kernel-with-mode oldval)
    ) ; end let
  )

(defun evawiz-ev-execute (arg)
  "Executes a line of code with the Evawiz kernel.
   If an argument is passed, excute expression form
   current point" 
  (interactive "P")
  (if (null arg) (setq arg t) (setq arg nil)) 
  (let ((kernel (evawiz-kernel-from-workbuf (current-buffer))) )
    (if (processp kernel)
        (let ((start) (end) (input))
          (undo-boundary)
          ;; find a command line
          ;; start from the beginning of the line
          ;; if a argument if passed, start from current point
          ;; and ingnore whether if it is connected with previous line
          (if arg (beginning-of-line))
          (setq start (point))
          ;; move point to end of the line, coninue if encounter \ + = ... [
          (setq end (evawiz-find-expression-end))
          (if (> (- end start) 0) 
              (progn
                ;; get the input
                (setq input (buffer-substring start end))
                (evawiz-evxecute-expression input t)
                (goto-char (+ end 1)) (skip-chars-forward " \t\n")
                ) ; end of progn
            (progn (forward-char) (skip-chars-forward " \t\n"))
            ) ; end of if input is empty
          ) ; end let
      (error "No Evawiz kernel exist!")
      ) ; end of if
    ) ; end of let
  )

(defun evawiz-ev-execute-all (arg)
  "Executes the whole .ev file"
  (interactive "P")
  (let ((kernel (evawiz-kernel-from-workbuf (current-buffer))) )
    (if (processp kernel)
        (let ((expr) )
          (save-buffer) 
          (if (not (buffer-file-name))
              (error "please save the buffer first."))
          (setq expr (concat "Get[\"" (buffer-file-name) "\"]"))
          (evawiz-evxecute-expression expr t)
          ) ; end let
      (error "This buffer has no Evawiz kernel!")
      ) ; end of if
    ) ; end of let
  )

(provide 'evawiz-ev)

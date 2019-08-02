
(custom-set-variables
 '(blink-cursor-mode nil)
 '(display-time-mode t)
 '(font-use-system-font t)
 '(transient-mark-mode (quote (only . t))))
(custom-set-faces
 '(default ((t (:family "DejaVu Sans Mono" :foundry "unknown" :slant normal :weight normal :height 99 :width normal)))))

(add-to-list 'default-frame-alist '(font . "-outline-Consolas-normal-normal-normal-mono-*-*-*-*-c-*-iso10646-1" ))
(set-face-attribute 'default t :font "-outline-Consolas-normal-normal-normal-mono-*-*-*-*-c-*-iso10646-1" )

(setq make-backup-files nil)
(setq backup-by-copying t)
(setq create-lockfiles nil)

;;;;;;;;;;;;;;;;;
;;;inital size
;;(setq default-frame-alist '((height . 35) (width . 135) ) )

;;;;;;;;;;;;;;;;;
;; turn welcome screen off
(add-hook 'after-init-hook 'close-welcome-buffer)
(defun close-welcome-buffer () "Close welcome buffer." (interactive) 
  (setq inhibit-startup-screen t)
  (if (bufferp "*GNU Emacs*")
      (progn 
        (kill-buffer "*GNU Emacs*")
        (message "buffer *GNU Emacs* is killed")
        ); end of progn
    (message "no buffer named *GNU Emacs* exist yet")
    ); end of if
 )

(setq-default indent-tabs-mode nil)
(show-paren-mode t)
(blink-cursor-mode (- (*) (*) (*)))
(setq linum-format "%3d  ")  ;set format  
(global-linum-mode 1) ; always show line number
;;(menu-bar-mode 1) ; hide menu bar
;;(tool-bar-mode 0) ; hide tool bar
(setq scroll-margin 3 scroll-conservatively 10000 )
;;(set-scroll-bar-mode nil)
(fset 'yes-or-no-p 'y-or-n-p) 
(setq backup-directory-alist (quote (("." . "~/.backups"))))
(setq kill-ring-max 200)
(setq require-final-newline t)
(mouse-avoidance-mode 'animate) 
(setq default-tab-width 2)
(setq tab-width 2)
(setq split-height-threshold 0)
(setq split-width-threshold nil)
(desktop-save-mode 0)
;;----------------------------------------------------------------------
(setq etc-dir (concat (getenv "EVAWIZ_ROOT") "/etc/emacs/") )
;;;--------------------------------------------------------------
;; to vim habit
(add-to-list 'load-path (concat etc-dir "evil") ) 
(require 'evil)
(evil-mode t)
(setq frame-title-format "evil-emacs@%b")
;; remember place
(require 'saveplace)
(setq-default save-place t) 
(setq save-place-file (concat etc-dir "evil-save") ) 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; auto complete
(add-to-list 'load-path (concat etc-dir "auto-complete") )
(require 'auto-complete-config)
(add-to-list 'ac-dictionary-directories (concat etc-dir "auto-complete/ac-dict") )
(ac-config-default)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(add-to-list 'load-path (concat etc-dir "modes") ) 
(require 'sql)
(require 'web-mode)
(require 'cuda)
(require 'keybinding)
(require 'evawiz)


(add-to-list 'ac-modes 'web-mode)
(add-to-list 'ac-modes 'evawiz-kernel-mode)
(add-to-list 'ac-modes 'evawiz-ev-mode)
(add-to-list 'ac-modes 'evawiz-multi-mode)

;(add-to-list 'auto-mode-alist '("\\.wxml\\'" . web-mode))
;(add-to-list 'auto-mode-alist '("\\.wxss\\'" . css-mode))

;;----------------------------------------------
;; gdb and compile binding

(setq compile-command "make -f Makefile")
(setq compile-install-command "make -f Makefile install")
(setq gud-gdb-command-name "cuda-gdb --annotate=3")
(setq gud-gdb-command-name "gdb -i=mi")

(defun gud-quit() (interactive) 
 (gud-call "quit")
 (sleep-for 0.5)
 (save-excursion
   (switch-by-str "*gud-") 
   (kill-buffer (current-buffer))
   (delete-window)
   )
 )

(defun shrink-compile-window()
  "shrink compile window, avoid compile window occupy 1/2 hight of whole window"
  (interactive)
  (setq compiled_buffer_name (buffer-name (current-buffer)))
  (switch-to-buffer-other-window "*compilation*")
  (if (< (/ (frame-height) 3.0) (window-height))
    (shrink-window (/ (window-height) 2)))
  (switch-to-buffer-other-window compiled_buffer_name)
  )
(defun du-onekey-compile ()
  "Save buffers and start compile"
  (interactive)
  (save-some-buffers t)
  (compile compile-command)
  (shrink-compile-window)
  )

(defun du-onekey-compile-install ()
  "Save buffers and start compile"
  (interactive)
  (save-some-buffers t)
  (compile compile-install-command)
  (shrink-compile-window)
  )

(defun gud-break-or-remove()
  "create a break point or remove it"
  (interactive)
  (save-excursion
	(if (eq (car (fringe-bitmaps-at-pos (point))) 'breakpoint)
		(gud-remove nil)
	  (gud-break nil))))
(defun gud-start() (interactive) (gud-call "start"))
(defun my-buffer-switch()
  "switch to most recently opened buffer"
  (interactive)
  (switch-to-buffer (other-buffer (current-buffer) t)))

(defun my-fullscreen ()
  (interactive)
  (x-send-client-message
    nil 0 nil "_NET_WM_STATE" 32
    '(2 "_NET_WM_STATE_FULLSCREEN" 0)
    )
  )
;;(if (display-graphic-p) (my-fullscreen))
;;switch to a buffer with part name matched prefix letters
(defun str-match (str1  str2)
  (interactive)
  (and (<= (string-width str1) (string-width str2))
		   (equal str1 (substring str2 0 (string-width str1) )))
  )
(defun buffer-name-by-str (bstr)
  (interactive)
  (setq bl (buffer-list))
  (setq ans nil)
  (while bl
         (setq bnn (buffer-name (setq bn (pop bl))))
         (if (str-match bstr bnn)
           (setq ans bnn)))
  ans
  )

(defun switch-by-str (bstr)
  (interactive) (switch-to-buffer-other-window (buffer-name-by-str bstr)))
(defun window-height-to (height)
  (interactive)
  (while (> (window-height) height) (shrink-window 1))
  (while (< (window-height) height) (shrink-window -1))
  )
(defun window-width-to-ratio(ratio)
  (interactive)
  (if (< (- (frame-width) 10) (window-width)) nil
    (while (> (/ (float (window-width)) (frame-width)) ratio) (shrink-window-horizontally 1))
    (while (< (/ (float (window-width)) (frame-width)) ratio) (shrink-window-horizontally -1))
    )
  )

(defun my-window-switch()
  "switch to most recently opened buffer"
  (interactive)
  (switch-to-buffer-other-window
   (buffer-name (other-buffer (current-buffer) t))))

(setq gdb-many-windows t)
(setq gdb-show-main t)
(setq gdb-use-separate-io-buffer t) 
;;"my gdb UI"
(defun gud-change-layout ()
  (interactive)
  (switch-by-str "*stack frames of") (window-height-to 5)
  (switch-by-str "*breakpoints of") (delete-window)
  (switch-by-str "*locals of")
  ;;(switch-by-str "*gud-") (window-height-to-ration 0.3)
  (switch-by-str "*gud-") 
  (window-width-to-ratio 0.7)
  (windmove-down) (window-width-to-ratio 0.7)
  )
(add-hook 'gdb-mode-hook 'gud-change-layout)

(defun gud-excu()
  (interactive)
  (if (str-match "*gud-" (buffer-name))
    ((lambda () (interactive)
       (comint-send-input)
       (my-window-switch ))
     )
    ((lambda () (interactive)
       (switch-by-str "*gud-")
       ))
    )
  )

(defun gud-mode-keys ()
  (local-set-key (kbd "<prior>") 'comint-previous-input)
  (local-set-key (kbd "<up>") 'comint-previous-input)
  (local-set-key (kbd "<next>")  'comint-next-input)
  (local-set-key (kbd "<down>")  'comint-next-input)
  ;...
  )
(add-hook 'gdb-mode-hook 'gud-mode-keys)
  
;; recent files
(require 'recentf)
(recentf-mode 1)
(setq recentf-max-menu-items 25)



(global-unset-key (kbd "M-b"))
(global-unset-key (kbd "M-r"))
(global-unset-key (kbd "M-s"))
(global-unset-key (kbd "M-n"))
(global-unset-key (kbd "M-i"))
(global-unset-key (kbd "M-f"))
(global-unset-key (kbd "M-u"))
(global-unset-key (kbd "M-p"))
(global-unset-key (kbd "C-M-p"))
(global-set-key (kbd "M-b") 'gud-break-or-remove)
(global-set-key (kbd "M-r") 'gud-go)
(global-set-key (kbd "M-s") 'gud-start)
(global-set-key (kbd "M-n") 'gud-next)
(global-set-key (kbd "M-i") 'gud-step)
(global-set-key (kbd "M-f") 'gud-finish)
(global-set-key (kbd "M-u") 'gud-until)
(global-set-key (kbd "M-p") 'gud-print)
(global-set-key (kbd "C-M-p") 'gud-pstar)

(global-set-key [(f2)] 'save-buffer )
(global-set-key (kbd "C-s") 'save-buffer)
(global-set-key "\C-h" 'backward-delete-char-untabify)  



(global-set-key "\C-x\ \C-r" 'recentf-open-files)

(global-set-key [(f3)] 'my-buffer-switch)
(global-set-key [(C-f3)] 'buffer-menu)

(winner-mode t)
(global-set-key (kbd "M-[") 'winner-undo)
(global-set-key (kbd "M-]") 'winner-redo)
(global-set-key [(control k)] 'delete-window ) 
(global-set-key [(control K)] 'kill-buffer-and-window)

(global-unset-key (kbd "M-h"))
(global-unset-key (kbd "M-j"))
(global-unset-key (kbd "M-k"))
(global-unset-key (kbd "M-l"))
(global-set-key (kbd "M-h") 'windmove-left) 
(global-set-key (kbd "M-j") 'windmove-down) 
(global-set-key (kbd "M-k") 'windmove-up) 
(global-set-key (kbd "M-l") 'windmove-right) 

(global-unset-key (kbd "M-e"))
(global-unset-key (kbd "M-E"))
(global-set-key (kbd "M-e") 'next-error)
(global-set-key (kbd "M-E") 'previous-error)

(global-set-key [(f5)] '(lambda ()
                          (interactive)
                          ;;(if (file-exists-p "main.cu")
                          ;    (call-interactively 'cuda-gdb)
                          ;  (call-interactively 'gdb)
                          ;  )
                          (call-interactively 'gdb)
                          (sleep-for 1.0)
                          (gud-change-layout) )
                )
(global-set-key [(C-f5)] 'gud-quit)
(global-set-key [(S-f9)] 'compile)
(global-set-key [(f9)] 'du-onekey-compile)
(global-set-key [(C-f9)] 'du-onekey-compile-install)
(global-set-key [f11] 'my-fullscreen) 
(global-set-key [(f10)] 'tool-bar-mode) 
(global-set-key [(S-f10)] 'menu-bar-mode) 
(global-set-key "\C-m" 'newline-and-indent)
(global-set-key (kbd "C-<return>") 'newline)
(global-set-key (kbd "M-<return>") 'gud-excu) 

(global-set-key [(f12)] 'semantic-ia-fast-jump)
(global-set-key [(S-f12)]
			 (lambda () (interactive)
			   (if (ring-empty-p (oref semantic-mru-bookmark-ring ring))
				(error "Semantic Bookmark ring is empty"))
			   (let* ((ring (oref semantic-mru-bookmark-ring ring))
					(alist (semantic-mrub-ring-to-assoc-list ring))
					(first (cdr (car alist))))
				(if (semantic-equivalent-tag-p (oref first tag)
										 (semantic-current-tag))
				  (setq first (cdr (car (cdr alist)))))
				(semantic-mrub-switch-tags first)
				)
			   )
			 )



(provide 'keybinding)

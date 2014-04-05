;;;; pasm.el

(defvar *pasm-mode-syntax-table* nil)
(defvar *pasm-mode-keymap* nil)
(defvar *pasm-label-regexp* "[A-Za-z_][A-Za-z_0-9]*:")

(unless *pasm-mode-syntax-table*
  (setq *pasm-mode-syntax-table* (make-syntax-table))
  (modify-syntax-entry ?\\ "\\" *pasm-mode-syntax-table*)
  (modify-syntax-entry ?\/ ". 12b" *pasm-mode-syntax-table*)
  (modify-syntax-entry ?\n "> b" *pasm-mode-syntax-table*)
  (modify-syntax-entry ?:  "_"  *pasm-mode-syntax-table*))

(unless *pasm-mode-keymap* 
  ;; please, someone tell how i should really do this...
  (let ((inner-keymap (make-sparse-keymap)))
    (setq *pasm-mode-keymap* (make-sparse-keymap))
    (define-key *pasm-mode-keymap* (kbd "\C-c") inner-keymap)
    (define-key *pasm-mode-keymap* (kbd "TAB") 'pasm-indent-function)))

(setq pasm-font-lock-keywords `(;; labels
                                ;; NB: i hearby decree that labels
                                ;; must be the first thing on a line,
                                ;; the assembler be damned.
                                (,(concat "^" *pasm-label-regexp*) . font-lock-constant-face)
                                ("^#include" . font-lock-preprocessor-face)
                                ("^#define" . font-lock-preprocessor-face)
                                ("^#ifdef" . font-lock-preprocessor-face)
                                ("^#ifndef" . font-lock-preprocessor-face)
                                ("^#else" . font-lock-preprocessor-face)
                                ("^#if" . font-lock-preprocessor-face)
                                ("^#endif" . font-lock-preprocessor-face)
                                ;; assembler directives
                                ("^\\s-*\\.[a-zA-Z]*" . font-lock-builtin-face)
                                ("\\<\\(jmp\\|JMP\\|jal\\|JAL\\|call\\|CALL\\|qbgt\\|QBGT\\|qbge\\|QBGE\\|qblt\\|QBLT\\|qble\\|QBLE\\|qbeq\\|QBEQ\\|qbne\\|QBNE\\|qba\\|QBA\\|qbbs\\|QBBS\\|qbbc\\|QBBC\\|wbs\\|WBS\\|wbc\\|WBC\\|lbco\\|LBCO\\|clr\\|CLR\\|sbco\\|SBCO\\|mov\\|MOV\\|sbbo\\|SBBO\\|halt\\|HALT\\|add\\|ADD\\|adc\\|ADC\\|sub\\|SUB\\|suc\\|SUC\\|rsb\\|RSB\\|rsc\\|RSC\\|lsl\\|LSL\\|lsr\\|LSR\\|and\\|AND\\|or\\|OR\\|xor\\|XOR\\|not\\|NOT\\|min\\|MIN\\|max\\|MAX\\|set\\|SET\\|scan\\|SCAN\\|lmbd\\|LMBD\\|ldi\\|LDI\\|mvib\\|MVIB\\|mviw\\|MVIW\\|mvid\\|MVID\\|lbbo\\|LBBO\\|lfc\\|LFC\\|stc\\|STC\\|zero\\|ZERO\\|ret\\|RET\\)\\>[^_a-zA-Z0-9]+" .
                                 font-lock-keyword-face)
                                ;; pasm registers
                                ("[^_]\\<\\(C\\|c\\)\\([0-9]+\\)\\>" .
                                 font-lock-variable-name-face)
                                ("[^_]\\<\\(r\\|R\\)\\(\\(\\(31\\|30\\).[a-zA-Z]\\([0-9]\\|[12][0-9]\\|3[01]\\)\\)\\|\\([0-9]\\|[0-9][0-9]\\)\\)\\>" .
                                 font-lock-variable-name-face)))
                                
(defun pasm-mode ()
  "Simple Emacs mode for editing Parrot Assembler"
  (interactive)
  (kill-all-local-variables)
  (setq major-mode 'pasm-mode)
  (setq mode-name "PASM")
  (set-syntax-table *pasm-mode-syntax-table*)
  (make-local-variable 'paragraph-start)
  (setq paragraph-start (concat "^$\\|" page-delimiter))
  (make-local-variable 'paragraph-separate)
  (setq paragraph-separate paragraph-start)
  (make-local-variable 'indent-line-function)
  (setq indent-line-function 'pasm-indent-line-function)
  (make-local-variable 'require-final-newline)
  (setq require-final-newline t)
  (make-local-variable 'comment-start)
  (setq comment-start "//")
  (make-local-variable 'comment-end)
  (setq comment-end "")
  (make-local-variable 'comment-start-skip)
  (setq comment-start-skip "#+ *")
  (make-local-variable 'font-lock-defaults)
  (setq font-lock-defaults '(pasm-font-lock-keywords))
  (font-lock-mode 1)
  (use-local-map *pasm-mode-keymap*)
  (run-hooks 'pasm-mode-hook))

(defun pasm-indent-line-function ()
  (interactive)
  (save-excursion
    (beginning-of-line)
    ;;(delete-region (point) (+ (point) (current-indentation)))
    (delete-horizontal-space)
    (cond
     ((looking-at "[A-Za-z_][A-Za-z_0-9]*:")
      (indent-to 0))
     (t
      (indent-to 8)))))

(defun beginning-of-line-point (&optional n)
  "Return the point at the beginning of the current line. N gets
passed to beginning-of-line if you want"
  (save-excursion 
    (beginning-of-line n)
    (point)))

(defun pasm-indent-function ()
  "This differs from pasm-indent-line-function in that if we end up at
the beginning of a line (which doesn't have a label) we want to be
moved forward to column 8"
  (interactive)
  (pasm-indent-line-function)
  ;; how do we check if we're at the beginning of a line? there must
  ;; be a function for this
  (unless (or (looking-at "[A-Za-z_][A-Za-z_0-9]*:") 
              (/= (beginning-of-line-point) (point)))
      (forward-char 8)))



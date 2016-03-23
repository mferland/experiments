(require 'package)
(require 'cl)
(package-initialize)

(add-to-list 'package-archives
             '("marmalade" . "https://marmalade-repo.org/packages/"))
(add-to-list 'package-archives
             '("melpa" . "https://melpa.org/packages/"))

(defvar autop-packages
  '(anzu
    auctex
    cmake-mode
    expand-region
    flycheck
    wgrep
    yasnippet
    zenburn-theme)
  "A list of packages to install at launch.")

(defun autop-packages-installed-p ()
  (loop for p in autop-packages
        when (not (package-installed-p p)) do (return nil)
        finally (return t)))

(unless (autop-packages-installed-p)
  ;; check for new packages (package versions)
  (message "%s" "Refreshing package database...")
  (package-refresh-contents)
  (message "%s" " done.")
  ;; install the missing packages
  (dolist (p autop-packages)
    (when (not (package-installed-p p))
      (package-install p))))

(provide 'auto-package)

all:		z-checker-report.pdf

z-checker-report.pdf:	z-checker-report.tex
		latexmk -ps z-checker-report
		ps2pdf z-checker-report.ps

again:		distclean
		@make all

clean:
		latexmk -c z-checker-report; rm *.pdf

distclean:
		latexmk -C z-checker-report

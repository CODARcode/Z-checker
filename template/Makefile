all:		z-checker-report.pdf

z-checker-report.pdf:	z-checker-report.tex
		latexmk -pdf z-checker-report

again:		distclean
		@make all

clean:
		latexmk -c z-checker-report; rm *.pdf

distclean:
		latexmk -C z-checker-report

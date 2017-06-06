all:		z-checker-report.pdf

z-checker-report.pdf:	z-checker-report.tex
		export PATH=$(PATH):../../../latexmk && latexmk -ps z-checker-report
		ps2pdf z-checker-report.ps

again:		distclean
		@make all

clean:
		export PATH=$(PATH):../../../latexmk && latexmk -c z-checker-report; rm *.pdf

distclean:
		export PATH=$(PATH):../../../latexmk && latexmk -C z-checker-report

all:pdf
pdf:report.tex
	pdflatex -interaction=nonstopmode report.tex
	bibtex report
	bibtex report
	pdflatex -interaction=nonstopmode report
	pdflatex -interaction=nonstopmode report
clean:
	rm -rf *.toc *.aux *.lof *.lot *.log *.blg *.bbl *.out

NAME = winter2013
TARGETPDF = $(NAME).pdf

CONFIG = $(NAME).yaml
REFILES = 00intro.re 019front80211.re \
	  88tips.re 99concl.re

# command
PDFMAKE :=  review-pdfmaker

$(TARGETPDF): $(CONFIG) $(REFILES)
	rm $(TARGETPDF) || echo "no pdf exists: clean build"
	$(PDFMAKE) $(CONFIG)

clean:
	rm $(TARGETPDF)

open: $(TARGETPDF)
	open $(TARGETPDF)

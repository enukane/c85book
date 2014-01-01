XXXX THIS README IS INCOMPLETE XXX

## なんだこれは！

C85, 2013冬コミで頒布した「マスタリング 802.11 on 9 : + 無線LAN的なTips」の
原稿データになります

## Requirements

- Ruby
- [ReVIEW](https://github.com/kmuto/review): flexible document format/conversion system.

To install required softwares, please type as follows after installing ruby environemt:

    gem install review


## ReVIEWへのパッチ

- Change TOC color from red to black (${REVIEWPATH}/lib/review/review.tex.erb)
```diff
@@ -41,6 +41,7 @@
 \usepackage[dvipdfm,bookmarks=true,bookmarksnumbered=true,colorlinks=true,%
             pdftitle={<%= values["booktitle"] %>},%
             pdfauthor={<%= values["aut"] %>}]{hyperref}
+\hypersetup{ linkcolor=black }

 \newenvironment{reviewimage}{%
   \begin{figure}[H]
```

## How to Build

just make it! Then ```winter2013.pdf``` is created in your winter2013book directory.

## File list
- 

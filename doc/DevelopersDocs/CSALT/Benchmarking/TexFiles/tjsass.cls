%%
%% This is file `tjsass.cls',
%% generated with the docstrip utility.
%%
%% The original source files were:
%%
%% jsclasses.dtx
%% 
%% IMPORTANT NOTICE:
%% 
%% For the copyright see the source file.
%% 
%% Any modified versions of this file must be renamed
%% with new filenames
%% 
%% For distribution of the original source see the terms
%% for copying and modification in the file jsclasses.dtx.
%% 
%% This generated file may be distributed as long as the
%% original source files, as listed above, are part of the
%% same distribution. (The sources need not necessarily be
%% in the same archive or directory.)
%% \CharacterTable
%%  {Upper-case    \A\B\C\D\E\F\G\H\I\J\K\L\M\N\O\P\Q\R\S\T\U\V\W\X\Y\Z
%%   Lower-case    \a\b\c\d\e\f\g\h\i\j\k\l\m\n\o\p\q\r\s\t\u\v\w\x\y\z
%%   Digits        \0\1\2\3\4\5\6\7\8\9
%%   Exclamation   \!     Double quote  \"     Hash (number) \#
%%   Dollar        \$     Percent       \%     Ampersand     \&
%%   Acute accent  \'     Left paren    \(     Right paren   \)
%%   Asterisk      \*     Plus          \+     Comma         \,
%%   Minus         \-     Point         \.     Solidus       \/
%%   Colon         \:     Semicolon     \;     Less than     \<
%%   Equals        \=     Greater than  \>     Question mark \?
%%   Commercial at \@     Left bracket  \[     Backslash     \\
%%   Right bracket \]     Circumflex    \^     Underscore    \_
%%   Grave accent  \`     Left brace    \{     Vertical bar  \|
%%   Right brace   \}     Tilde         \~}
%\NeedsTeXFormat{pLaTeX2e}
\ProvidesClass{tjsass}
[2016/05/30 modified by H. Kojima TMU]
\newif\if@restonecol
\newif\if@titlepage
\newif\iftitleheader
%%
\def\@NAME#1#2/{\uppercase{#1}\lowercase{#2}\ }%
\def\@@NAME#1#2/{\uppercase{#1}{\fontsize{8pt}{9pt}\selectfont\uppercase{#2}}}%
\def\NAME#1#2{\expandafter\@NAME#1/%
\expandafter\@@NAME#2/}%
%%
%%%%
\def\ThanksLists{}%
\newcounter{@ThanksCounts}%
\setcounter{@ThanksCounts}{0}%
\def\TLset#1#2{\begingroup\let\tempCS=\relax
\xdef#1{#1\tempCS#2}\endgroup}%
\def\ADDtoTL#1{\TLset\ThanksLists{{#1}}}%
%%
\def\thanksNum#1{$^{#1}$}%
% 所属の記述
\def\thanksOrg#1{%
\begingroup
\ADDtoTL{#1}%
\def\protect{\noexpand\protect\noexpand}\xdef\@thanksOrg{\@thanksOrg%
}\endgroup}
\def\@thanksOrg{}
%%
\def\buildThanks#1{
  \stepcounter{@ThanksCounts}%
  \begin{center}%
  \fontsize{8pt}{8.5pt}\selectfont$^{\the@ThanksCounts)}$\textit{#1}%
  \end{center}%
}
%%from indent.sty
\newenvironment{indentation}[2]%
{%
  \par
  \setlength{\leftmargin}{#1}%
  \setlength{\rightmargin}{#2}%
  \advance\linewidth -\leftmargin
  \advance\linewidth -\rightmargin
  \advance\@totalleftmargin\leftmargin
  \@setpar{{\@@par}}%
  \parshape 1 \@totalleftmargin \linewidth
  \ignorespaces
}%
{\par}
\newenvironment{ctrindent}[2]%
{\begin{indentation}{#1}{#2}\centering}%
{\end{indentation}}
%%
\newif\ifpubform
\DeclareOption{pubform}{\pubformtrue}%

\newif\iftjsass%
\DeclareOption{TJSASS}{\tjsasstrue}%

\newif\ifatj%
\DeclareOption{ATJ}{\atjtrue}%

\newif\ifists%
\DeclareOption{ISTS}{\iststrue}%

\newif\ifapisat
\DeclareOption{APISAT}{\apisattrue}%

\DeclareOption{a4paper}{%
  %\setlength\paperheight {297mm}%
  \setlength\paperwidth  {210mm}}
\DeclareOption{a4j}{%
  \setlength\paperheight {297mm}%
  \setlength\paperwidth  {210mm}}

\newcommand{\@ptsize}{10}
\DeclareOption{8pt}{\renewcommand{\@ptsize}{8}}
\DeclareOption{8.5pt}{\renewcommand{\@ptsize}{8.5}}
\DeclareOption{9pt}{\renewcommand{\@ptsize}{9}}
\DeclareOption{9.5pt}{\renewcommand{\@ptsize}{9.5}}
\DeclareOption{10pt}{\renewcommand{\@ptsize}{10}}
\DeclareOption{11pt}{\renewcommand{\@ptsize}{11}}
\DeclareOption{12pt}{\renewcommand{\@ptsize}{12}}
\DeclareOption{14pt}{\renewcommand{\@ptsize}{14}}
\DeclareOption{20pt}{\renewcommand{\@ptsize}{20}}
\DeclareOption{landscape}{%
  \setlength\@tempdima   {\paperheight}%
  \setlength\paperheight {\paperwidth}%
  \setlength\paperwidth  {\@tempdima}}
\DeclareOption{oneside}{\@twosidefalse \@mparswitchfalse}
\DeclareOption{twoside}{\@twosidetrue \@mparswitchtrue}
\DeclareOption{twocolumns}{\@twocolumntrue}
\DeclareOption{titlepage}{\@titlepagetrue}
\DeclareOption{notitlepage}{\@titlepagefalse}
\DeclareOption{leqno}{\input{leqno.clo}}
\DeclareOption{fleqn}{\input{fleqn.clo}}
\newif\ifdraft
\DeclareOption{draft}{\drafttrue \setlength\overfullrule{5pt}}
\DeclareOption{final}{\draftfalse \setlength\overfullrule{0pt}}
\newif\ifjisfont
\jisfonttrue
\DeclareOption{mingoth}{\jisfontfalse}
\ExecuteOptions{a4paper,twoside,twocolumns,notitlepage,final}
%
\ProcessOptions
\AtBeginDvi{\special{papersize=210mm,297mm}}
\AtBeginDvi{\special{pdf: pagesize width 210mm height 297mm}}
\def\n@baseline{12}%
\ifnum\@ptsize=9
  \mag 900
  \setlength\paperwidth{1.11111\paperwidth}%
  \setlength\paperheight{1.11111\paperheight}%
  \def\n@baseline{15}%
\fi
\ifnum\@ptsize=9.5
  \mag 950
  \setlength\paperwidth{1.05263\paperwidth}%
  \setlength\paperheight{1.05263\paperheight}%
\fi
\ifnum\@ptsize=10
  \mag 1000
  \setlength\paperwidth{1.0\paperwidth}%
  \setlength\paperheight{1.0\paperheight}%
\fi
\ifnum\@ptsize=11
  \mag 1100
  \setlength\paperwidth{0.90909\paperwidth}%
  \setlength\paperheight{0.90909\paperheight}%
\fi
\ifnum\@ptsize=12
  \mag 1200
  \setlength\paperwidth{0.83333\paperwidth}%
  \setlength\paperheight{0.83333\paperheight}%
\fi
\ifnum\@ptsize=20
  \mag 2000
  \setlength\paperwidth{0.5\paperwidth}%
  \setlength\paperheight{0.5\paperheight}%
\fi
\DeclareRobustCommand\sffamily
        {\not@math@alphabet\sffamily\mathsf
         \fontfamily\sfdefault\selectfont}
\DeclareRobustCommand\ttfamily
        {\not@math@alphabet\ttfamily\mathtt
         \fontfamily\ttdefault\selectfont}
\DeclareTextCommand{\textsterling}{OT1}{{\itshape\char`\$}}
\def\@{\spacefactor3000\space}
\def\@setfontsize#1#2#3{%
  \ifx\protect\@typeset@protect
    \let\@currsize#1%
  \fi
  \fontsize{#2}{#3}\selectfont
  \ifdim\parindent>\z@
    \parindent=1em
  \fi
}
\newif\ifnarrowbaselines
\def\narrowbaselines{\narrowbaselinestrue\@currsize\selectfont}
\renewcommand{\normalsize}{%
  \ifnarrowbaselines
    \@setfontsize\normalsize\@xpt\@xipt
  \else
	\iftjsass
    	\@setfontsize\normalsize{10pt}{\n@baseline}%
	\else
    	\@setfontsize\normalsize{9.5pt}{\n@baseline}%
	\fi
  \fi
  \abovedisplayskip .6\baselineskip \@plus.6\baselineskip \@minus.2\baselineskip
  \abovedisplayshortskip \abovedisplayskip
  \belowdisplayshortskip .4\baselineskip \@plus.4\baselineskip \@minus.1\baselineskip
  \belowdisplayskip \abovedisplayskip
\let\@listi\@listI}
\normalsize%
\newcommand{\bibfontsize}{%
  \ifnarrowbaselines
    \@setfontsize\bibfontsize{8\p@}\@ixpt%
  \else
    \@setfontsize\bibfontsize{8\p@}{10}%
  \fi
  \abovedisplayskip 10\p@ \@plus2\p@ \@minus5\p@
  \abovedisplayshortskip \z@ \@plus3\p@
  \belowdisplayshortskip 6\p@ \@plus3\p@ \@minus3\p@
  \belowdisplayskip \abovedisplayskip
  \let\@listi\@listI}
\newcommand{\small}{%
  \ifnarrowbaselines
    \@setfontsize\small\@ixpt{11}%
  \else
    \iftjsass
      \@setfontsize\small{9\p@}{11.5}%
	\else
     \@setfontsize\small{8\p@}{11.5}%
	\fi
  \fi
  \abovedisplayskip 8.5\p@ \@plus3\p@ \@minus4\p@
  \abovedisplayshortskip \z@ \@plus2\p@
  \belowdisplayshortskip 4\p@ \@plus2\p@ \@minus2\p@
  \def\@listi{\leftmargin\leftmargini
              \topsep \z@
              \parsep \z@
              \itemsep \parsep}%
  \belowdisplayskip \abovedisplayskip}
\newcommand{\footnotesize}{%
  \ifnarrowbaselines
    \@setfontsize\footnotesize\@viiipt{9.5}%
  \else
    \@setfontsize\footnotesize\@viiipt{11}%
  \fi
  \abovedisplayskip 6\p@ \@plus2\p@ \@minus4\p@
  \abovedisplayshortskip \z@ \@plus\p@
  \belowdisplayshortskip 3\p@ \@plus\p@ \@minus2\p@
  \def\@listi{\leftmargin\leftmargini
              \topsep \z@
              \parsep \z@
              \itemsep \parsep}%
  \belowdisplayskip \abovedisplayskip}
\newcommand{\scriptsize}{\@setfontsize\scriptsize\@viipt\@viiipt}
\newcommand{\tiny}{\@setfontsize\tiny\@vpt\@vipt}
\if@twocolumn
  \newcommand{\large}{\@setfontsize\large\@xiipt{\n@baseline}}
\else
  \newcommand{\large}{\@setfontsize\large\@xiipt{17}}
\fi
\newcommand{\Large}{\@setfontsize\Large\@xivpt{17.5}}
\newcommand{\LARGE}{\@setfontsize\LARGE\@xviipt{25}}
\newcommand{\huge}{\@setfontsize\huge\@xxpt{28}}
\newcommand{\Huge}{\@setfontsize\Huge\@xxvpt{33}}
\everydisplay=\expandafter{\the\everydisplay \narrowbaselines}
\newcommand{\headfont}{\sffamily\bfseries\fontseries{m}\selectfont}
\iftjsass
\setlength\columnsep{5mm}
\else
\setlength\columnsep{7.4mm}
\fi
\setlength\columnseprule{0\p@}
\setlength\lineskip{1\p@}
\setlength\normallineskip{1\p@}
\setlength\lineskiplimit{1\p@}
\setlength\normallineskiplimit{1\p@}
\renewcommand{\baselinestretch}{}
\setlength\parskip{0\p@}
\setlength\parindent{2em}
\@lowpenalty   51
\@medpenalty  151
\@highpenalty 301
\setlength\topskip{9.5\p@}
\setlength\headheight{10mm}
%\setlength\footskip{9mm}
\iftjsass
%\setlength\footskip{15mm}
\setlength\footskip{13mm}
\else
\setlength\footskip{12mm}
\fi
\setlength\headsep{4mm}
\setlength\maxdepth{.5\topskip}
\newdimen\fullwidth
\iftjsass
\setlength\fullwidth{171mm}
\addtolength\fullwidth{5mm}
\else
\setlength\fullwidth{169mm}
\addtolength\fullwidth{7mm}
\fi

\setlength\textwidth{\fullwidth}
\setlength{\textheight}{58\baselineskip}
\addtolength{\textheight}{\topskip}%
\setlength\marginparsep{1em}
\setlength\marginparpush{5\p@}
\setlength{\oddsidemargin}{\paperwidth}
\addtolength{\oddsidemargin}{-\textwidth}
\setlength{\oddsidemargin}{0.5\oddsidemargin}
\addtolength{\oddsidemargin}{-1truein}
\setlength{\evensidemargin}{\oddsidemargin}
\addtolength{\evensidemargin}{\fullwidth}
\addtolength{\evensidemargin}{-\textwidth}
\setlength\marginparwidth{\evensidemargin}
%\addtolength\marginparwidth{1truein}
%\addtolength\marginparwidth{-1cm}
\addtolength\marginparwidth{-\marginparsep}
\@tempdima=1em
\divide\marginparwidth\@tempdima
\multiply\marginparwidth\@tempdima
\setlength\topmargin{24.1mm}% \topskip込みで上余白約25mm
\addtolength\topmargin{-\headheight}
\addtolength\topmargin{-\headsep}
\addtolength\topmargin{-1truein}
{\footnotesize\global\setlength\footnotesep{\baselineskip}}
%\setlength\footnotesep{0.7\footnotesep}
\setlength\footnotesep{0.6\footnotesep}
\setlength{\skip\footins}{16\p@ \@plus 5\p@ \@minus 10\p@}
\setcounter{topnumber}{2}
\renewcommand{\topfraction}{.9}
\setcounter{bottomnumber}{2}
\renewcommand{\bottomfraction}{.9}
\setcounter{totalnumber}{5}
\renewcommand{\textfraction}{.1}
\renewcommand{\floatpagefraction}{.9}
\setcounter{dbltopnumber}{3}
\renewcommand{\dbltopfraction}{.9}
\renewcommand{\dblfloatpagefraction}{.9}
%\setlength\floatsep    {12\p@ \@plus 2\p@ \@minus 2\p@}
%\setlength\textfloatsep{20\p@ \@plus 2\p@ \@minus 4\p@}
%\setlength\intextsep   {12\p@ \@plus 2\p@ \@minus 2\p@}
\setlength\floatsep    {8\p@ \@plus 6\p@ \@minus 2\p@}
\setlength\textfloatsep{8\p@ \@plus 12\p@ \@minus 4\p@}
\setlength\intextsep   {6\p@ \@plus 8\p@ \@minus 2\p@}
%\setlength\dblfloatsep    {12\p@ \@plus 2\p@ \@minus 2\p@}
%\setlength\dbltextfloatsep{20\p@ \@plus 2\p@ \@minus 4\p@}
\setlength\dblfloatsep    {8\p@ \@plus 6\p@ \@minus 2\p@}
\setlength\dbltextfloatsep{6\p@ \@plus 8\p@ \@minus 2\p@}
\setlength\@fptop{0\p@ \@plus 1fil}
\setlength\@fpsep{8\p@ \@plus 2fil}
\setlength\@fpbot{0\p@ \@plus 1fil}
\setlength\@dblfptop{0\p@ \@plus 1fil}
\setlength\@dblfpsep{8\p@ \@plus 2fil}
\setlength\@dblfpbot{0\p@ \@plus 1fil}
\newdimen\abstractboxwidth
\iftjsass
\setlength\abstractboxwidth{152mm}
\else
\setlength\abstractboxwidth{150mm}
\fi
\newcommand*{\makecopyright}{%
\ifpubform
	\iftjsass
 	\bgroup
 	\renewcommand*{\thefootnote}{}
    \footnotetext{\normalfont\footnotesize{\copyright\,{\@pubyear} The Japan Society for Aeronautical and Space Sciences}}\setcounter{footnote}{0}
 	\@ifundefined{@confinfo}
 	{\footnotetext{\normalfont\footnotesize{$^*$Received \@ifundefined{@receiveddate}{}{\@receiveddate}; final revision received \@ifundefined{@revisiondate}{}{\@revisiondate}; accepted for publication \@ifundefined{@accepteddate}{}{\@accepteddate}}}
 	}
 	{\footnotetext{\normalfont\footnotesize{$^*$\@confinfo}}
  	\footnotetext{\normalfont\footnotesize{Received \@ifundefined{@receiveddate}{}{\@receiveddate}; final revision received \@ifundefined{@revisiondate}{}{\@revisiondate}; accepted for publication \@ifundefined{@accepteddate}{}{\@accepteddate}}}
 	}
 	\footnotetext{\normalfont\footnotesize $^\dag$Corresponding author, \@ifundefined{@email}{}{\@email}}
  	\egroup%
	\setcounter{footnote}{2}
	\fi
	\ifatj
 	\bgroup
 	\renewcommand{\thefootnote}{}
 	\footnotetext{\hspace*{-1.8em}\copyright\,{\@pubyear} The Japan Society for Aeronautical and Space Sciences}
 	\egroup%
	\setcounter{footnote}{0}
	\fi%
	\ifapisat
	\bgroup
 	\renewcommand{\thefootnote}{}
 	\footnotetext{\normalfont\footnotesize{\copyright\,{\@pubyear} The Japan Society for Aeronautical and Space Sciences}}\setcounter{footnote}{0}
 	\@ifundefined{@confinfo}{}
 	{\footnotetext{\normalfont\footnotesize{$^*$\@confinfo}} \setcounter{footnote}{1}}
 	\egroup%
	\fi
\else
  \iftjsass 
	\bgroup
	\renewcommand{\thefootnote}{}
 	\footnotetext{\normalfont\footnotesize{\copyright\,{\@pubyear} The Japan Society for Aeronautical and Space Sciences}}
	\footnotetext{\normalfont\footnotesize $^\dag$Corresponding author, \@ifundefined{@email}{}{\@email}}
	\egroup%
	\setcounter{footnote}{2}
  \fi
  %\bgroup\renewcommand{\thefootnote}{}\footnotetext{\hspace*{-1.8em}\phantom{B}}\setcounter{footnote}{0}\egroup%
\fi%
}%

\newcommand*{\ctgtitle}[1]{\gdef\@ctgtitle{#1}}%カテゴリタイトル

\iftjsass \renewcommand*{\title}[1]{\gdef\@title{#1$^*$}} \fi
\ifatj
  \ifpubform
    \renewcommand*{\title}[1]{\@ifundefined{@confinfo}{\gdef\@title{#1}}{\gdef\@title{#1$^*$}}}
  \else
    \renewcommand*{\title}[1]{\gdef\@title{#1}}
  \fi
\fi
\ifists \renewcommand*{\title}[1]{\gdef\@title{#1}} \fi
\ifapisat
  \ifpubform
    \renewcommand*{\title}[1]{\@ifundefined{@confinfo}{\gdef\@title{#1}}{\gdef\@title{#1$^*$}}}
  \else
    \renewcommand*{\title}[1]{\gdef\@title{#1}}
  \fi
\fi

\newcommand*{\headertitle}[1]{\gdef\@headertitle{#1}}%ヘッダ用タイトル
\newcommand*{\subtitle}[1]{\gdef\@subtitle{#1}}%サブタイトル
\renewcommand*{\author}[1]{\gdef\@author{#1}}%著者名
\newcommand{\CorresAuthor}[1]{\iftjsass $^\dag${\email{#1}}\,\fi}
\newcommand*{\headerauthor}[1]{\gdef\@headerauthor{#1}}%ヘッダ用著者名
\newcommand*{\receiveddate}[1]{\gdef\@receiveddate{#1}}%ヘッダ用著者名
\newcommand*{\revisiondate}[1]{\gdef\@revisiondate{#1}}%修正原稿受理日
\newcommand*{\accepteddate}[1]{\gdef\@accepteddate{#1}}%掲載決定日
%\newcommand*{\address}[1]{\gdef\@address{#1}}%所属機関住所
\newcommand*{\email}[1]{\gdef\@email{#1}}%メールアドレス
\newcommand*{\keywords}[1]{\gdef\@keywords{#1}}%キーワード
\newcommand*{\pubyear}[1]{\gdef\@pubyear{#1}}%発行年
\newcommand*{\bookvolume}[1]{\gdef\@bookvolume{#1}}%巻
\newcommand*{\bookissue}[1]{\gdef\@bookissue{#1}}%号
\newcommand*{\session}[1]{\gdef\@session{P#1\underline{~~}}}%セッション名
\newcommand*{\titleheader}[1]{\gdef\@titleheader{#1}}%タイトルヘッダー
\newcommand{\confinfo}[1]{\gdef\@confinfo{#1}}% presentation conference information
\newcommand{\AEname}[1]{\gdef\@aename{#1}}
\newcommand{\AEnameShow}{\ifpubform \iftjsass \vspace*{1ex} \hfill \@aename \par \hfill \textit{Associate Editor} \fi \fi} % Associate Editor name

\ctgtitle{}
%%%%
%%%% ヘッダ書式
%%%%
%%%
\@ifundefined{LastPage}{\gdef\LastPage{0}}{}%
\def\writeLastPage{\if@filesw\immediate\write\@auxout{%
\string\gdef\string\LastPage{\number\count0}}\fi}%
\let\origenddocument=\enddocument%
\def\enddocument{\writeLastPage\origenddocument}%
%%%
\def\@pagerange{%
\ifists
	\@ifundefined{LastPage}{}{%
		\ifnum\thepage=\LastPage\relax%
   			\LastPage\thepage
		\else{\@session}\thepage\,-\,{\@session}\LastPage,\fi
  	}
\else
    \ifapisat
	  \@ifundefined{LastPage}{}{%
		\ifnum\thepage=\LastPage\relax
   			\LastPage\thepage
		\else a\thepage\,-\,a\LastPage,\fi
  	  }
	\else
	  \@ifundefined{LastPage}{}{%
	  \ifnum\thepage=\LastPage\relax%
   		\LastPage\thepage
	  \else\thepage\,-\,\LastPage,\fi
      }
    \fi
\fi
}%

%%%%%%%%%%%
\newcommand*{\journalname}[1]{{\gdef\@journalname{#1}}}
\iftjsass 
  \journalname{Trans. Japan Soc. Aero. Space Sci.}
\else
  \journalname{Trans. JSASS Aerospace Tech. Japan}
\fi

%%%%%%%%%%%%%%
\newcommand*{\pagenullheader}{
  \def\@evenfoot{\hfill\normalfont\small \phantom{A}\hfill}%
  \def\@oddfoot{\hfill \normalfont\small \phantom{A}\hfill}%
  \def\@evenhead{\hfill\normalfont\small \phantom{A}\hfill}%
  \def\@oddhead{\hfill \normalfont\small \phantom{A}\hfill}%
}

\newcommand*{\tjfootheader}{
  \def\@oddfoot{\normalfont\footnotesize \copyright \@pubyear\, JSASS \hfill \thepage\hspace{1.75cm}\hfill}%
  \def\@evenfoot{\normalfont\footnotesize \copyright \@pubyear\, JSASS \hfill \thepage\hspace{1.75cm}\hfill}%
  \def\@evenhead{\hfill\normalfont\small \@journalname, Vol.\,{\@bookvolume}, No.~{\@bookissue},\,\@pubyear\hfill}%
  \def\@oddhead{\hfill\normalfont\small \@journalname, Vol.\,{\@bookvolume}, No.~{\@bookissue},\,\@pubyear\hfill}%
}

\newcommand*{\atjfootheader}{
	\def\@oddfoot{ \hfill \normalfont\small \thepage\hfill}%
	\def\@evenfoot{\hfill \normalfont\small \thepage\hfill}%
	\def\@evenhead{\hfill \normalfont\small \@journalname~Vol.\,{\@bookvolume} (\@pubyear)\hfill}%
	\def\@oddhead{\hfill \normalfont\small \@journalname~Vol.\,{\@bookvolume} (\@pubyear)\hfill}%
%	\def\@oddhead{ \hfill \normalfont\small \@ifundefined{@headerauthor}{:}{\@headerauthor:} {\@ifundefined{@headertitle}{\@title}{\@headertitle}}\hfill}
}

\newcommand*{\istsfootheader}{
    \def\@oddfoot{ \hfill \normalfont\small{\@session}\thepage\hfill}%
    \def\@evenfoot{\hfill \normalfont\small{\@session}\thepage\hfill}%
    \def\@evenhead{\hfill \normalfont\small \@journalname~Vol.\,{\@bookvolume}, No.{\@bookissue}~(\@pubyear)\hfill}%
    \def\@oddhead{\hfill \normalfont\small \@journalname~Vol.\,{\@bookvolume}, No.{\@bookissue}~(\@pubyear)\hfill}%
%    \def\@oddhead{ \hfill\ \normalfont\small \@ifundefined{@headerauthor}{:}{\@headerauthor:} {\@ifundefined{@headertitle}{\@title}{\@headertitle}}\hfill}
}
	
\newcommand*{\apisatfootheader}{
	\def\@oddfoot{ \hfill \normalfont\small a\thepage\hfill}%
	\def\@evenfoot{\hfill \normalfont\small a\thepage\hfill}%
    \def\@evenhead{\hfill \normalfont\small \@journalname~Vol.\,{\@bookvolume}, No.{\@bookissue} (\@pubyear)\hfill}
    \def\@oddhead{\hfill \normalfont\small \@journalname~Vol.\,{\@bookvolume}, No.{\@bookissue} (\@pubyear)\hfill}
%	\def\@oddhead{ \hfill \normalfont\small \@ifundefined{@headerauthor}{:}{\@headerauthor:} {\@ifundefined{@headertitle}{\@title}{\@headertitle}}\hfill}
}

%%%
\def\ps@headings{%
\ifpubform
  \iftjsass \tjfootheader   \fi
  \ifatj    \atjfootheader  \fi
  \ifists   \istsfootheader \fi
  \ifapisat \apisatfootheader \fi
\else%
  \pagenullheader
\fi%
}%

\def\ps@titleheadings{%
\ifpubform
\iftjsass
  \def\@oddfoot{\normalfont\footnotesize \hfill\thepage\hfill}%
  \def\@evenfoot{\normalfont\footnotesize \hfill\thepage\hfill}%
  \def\@oddhead{\parbox[b]{10cm}{\normalfont\footnotesize \@journalname \hfill\\Vol.\,{\@bookvolume}, No.\,{\@bookissue}, pp.\@pagerange\@pubyear\hfill}}%
  \def\@evenhead{\parbox[b]{10cm}{\normalfont\footnotesize \@journalname \hfill\\Vol.\,{\@bookvolume}, No.\,{\@bookissue}, pp.\@pagerange\@pubyear\hfill}}%
\fi
\ifists%
  \def\@oddfoot{
  \vspace*{6mm}
  \vbox to 8\baselineskip{\hsize\fullwidth
  \hrule width 1.0\columnwidth depth 0.5mm height 0mm
  \vspace*{3.5mm}
  \normalfont\small Copyright\copyright\,{\@pubyear} by the Japan Society for Aeronautical and Space Sciences and ISTS. All rights reserved.\\	\\ \hspace*{\fill} {\@session}\thepage\hspace*{\fill} }}%
  \def\@evenfoot{\vspace*{8mm} \\ \\ \hspace*{fill} {\@session}\thepage\hfill}%
  \def\@oddhead{\parbox[b]{10cm}{\normalfont\small \@journalname \hfill\\Vol.\,{\@bookvolume}, No.\,{\@bookissue}, pp.\@pagerange \@pubyear\hfill}}%
  \def\@evenhead{\parbox[b]{10cm}{\normalfont\small \@journalname \hfill\\Vol.\,{\@bookvolume}, No.\,{\@bookissue}, pp.\@pagerange \@pubyear\hfill}}%
\fi
\ifatj
  \def\@oddfoot{\hfill \normalfont\small \thepage\hfill}%
  \def\@evenfoot{\hfill \normalfont\small \thepage\hfill}%
  \def\@oddhead{\parbox[b]{10cm}{\normalfont\small \@journalname \hfill\\Vol.\,{\@bookvolume}, pp.\@pagerange \@pubyear\hfill}}%
  \def\@evenhead{\parbox[b]{10cm}{\normalfont\small \@journalname \hfill\\Vol.\,{\@bookvolume}, pp.\@pagerange \@pubyear\hfill}}%
\fi%
\ifapisat
  \def\@oddfoot{\normalfont\small \hfill a\thepage\hfill}%
  \def\@evenfoot{\normalfont\small \hfill a\thepage\hfill}%
  \def\@oddhead{\parbox[b]{10cm}{\normalfont\small \@journalname \hfill\\Vol.\,{\@bookvolume}, No.\,{\@bookissue}, pp.\@pagerange \@pubyear\hfill}}%
  \def\@evenhead{\parbox[b]{10cm}{\normalfont\small \@journalname \hfill\\Vol.\,{\@bookvolume}, No.\,{\@bookissue}, pp.\@pagerange \@pubyear\hfill}}%
\fi
\else%
  \pagenullheader
\fi%
}
%%%
\def\plainifnotempty{%
  \ifx \@oddhead \@empty
    \ifx \@oddfoot \@empty
    \else
      \thispagestyle{plain}%
    \fi
  \else
    \thispagestyle{plain}%
  \fi}
\newcommand{\maketitle}{\par%
  \begingroup
    \newdimen\title@ht%
    \newcount\LineNum%
    \newbox\@titlebox
    \twocolumn[\setbox\@titlebox=\vbox{\@maketitle}%
    \title@ht=\ht\@titlebox%
    \divide\title@ht by \baselineskip
    \LineNum=\title@ht
    \advance\LineNum by 1
    \title@ht=\LineNum\baselineskip
    \ht\@titlebox=\title@ht
    \copy\@titlebox]
	%%%%%%%%%%
    \thispagestyle{titleheadings}
%--------------------------
    \long\def\@makefntext##1{%
    \advance\leftskip 4mm
    \parindent 1em\noindent
    \llap{\@textsuperscript{\normalfont\@thefnmark}}##1}%
%--------------------------
    \iftjsass \makecopyright \fi
    \ifatj    \makecopyright \fi
    \ifapisat \makecopyright \fi
    \@thanks
    %%%%%%%%%%
  \endgroup
  \renewcommand*{\thefootnote}{\fnsymbol{footnote}}
  %\setcounter{footnote}{0}%
%----------------------
  \long\def\@makefntext##1{%
  \advance\leftskip 1.8em
  \parindent 1em\noindent
  \llap{\@textsuperscript{\normalfont\@thefnmark}}\hskip0.3em##1}%
%%-------------------------------
  \global\let\thanks\relax
  \global\let\maketitle\relax
  \global\let\@thanks\@empty
  \global\let\@author\@empty
  \global\let\@date\@empty
  \global\let\title\relax
  \global\let\ctgtitle\relax
  \global\let\subtitle\relax
  \global\let\author\relax
  \global\let\date\relax
  \global\let\and\relax
  \global\let\@email\relax
}
%

\newcommand*{\receivedateshow}{
\vspace*{.4\baselineskip} \@ifundefined{@receiveddate}{}{{\small(Received \@receiveddate)}}\par \vspace*{.6\baselineskip} 
}

\def\@maketitle{%
  \newpage\null
  \ifx\@ctgtitle\@empty\relax\vspace*{-2mm}{\phantom{nocategory}}\par\else
    \vspace*{-2mm}%
    {\usefont{T1}{txr}{b}{it}\ifpubform 
    \iftjsass \@ctgtitle \fi
    \ifatj    \@ctgtitle \fi
    \ifists   \@ctgtitle \fi
    \else
    	\phantom{Original}\fi}\par
  	\fi
  \vspace*{-5.5mm}%
  \begin{center}
  \ifx\@title\@empty\else
    {\Large\mbox{}\\\textbf{\@title}\par}%タイトル上に1行強制空白
  \fi%
  \ifx\@subtitle\@empty\else
  \vspace*{2mm}%
    {\Large\@subtitle\par}%
  \fi%
  \vspace*{5mm}
  \end{center}
  \begin{center}%
  \ifx\@title\@author\else%
   \iftjsass \fontsize{10\p@}{5mm}\selectfont{\@author}\par  
   \else
   \fontsize{10\p@}{5mm}\selectfont{By\ \@author}\par
   \fi
   \vspace*{2.3mm}%
    {\let\tempCS=\buildThanks{\ThanksLists}}%
    %\vspace*{1.5mm}%
  \fi
  \end{center}
  \ifx\@receiveddate\@empty\else%
  	\begin{center}%
  	    \ifatj    \receivedateshow \fi
  	    \ifists   \receivedateshow \fi
    	\ifapisat \receivedateshow \fi
  	\end{center}%
  \fi
  \begin{center}
  \ifvoid\@abstractbox
  \else
  %\vspace*{.5\baselineskip}
    \centerline{\box\@abstractbox}
  \vspace*{1.0\baselineskip}
  \fi
  \ifx\@keywords\@empty
  \else
  \vspace*{0.5\baselineskip}
  \setbox0=\hbox{\textrm{\small\textbf{Key Words:}\hspace{2ex}}}
  \begin{minipage}[u]{\abstractboxwidth}\par
  \begin{center}
    \advance\leftskip\wd0
    \noindent%
    \llap{\copy0}\normalfont\small\@keywords%
  \end{center}
  \end{minipage}
  \fi
  \end{center}%
  \vspace*{2mm}
  \vskip 1mm%タイトル最下部から本文までの空白追加
  \renewcommand\thefootnote{\@empty}
  \protect\footnotetext{A Guideline for Contributors to ISTS Preprint and The ISTS Special Issue of Transactions of JSASS A Guideline for Contributors to ISTS Preprint and The ISTS Special Issue of Transactions of JSASS A Guideline for Contributors to ISTS Preprint and The ISTS Special Issue of Transactions of JSASS}%
  }
  
%%%%%%%%%%
\def\@startsection#1#2#3#4#5#6{%
  \if@noskipsec \leavevmode \fi
  \par
  \@tempskipa #4\relax
  \@afterindenttrue
  \ifdim \@tempskipa <\z@
    \@tempskipa -\@tempskipa \@afterindentfalse
  \fi
  \if@nobreak
    \everypar{}%
  \else
    \addpenalty\@secpenalty
    \ifdim \@tempskipa >\z@
      \null
      \vspace*{-\baselineskip}\vskip\@tempskipa
    \fi
    \noindent
  \fi
  \@ifstar
    {\@ssect{#3}{#4}{#5}{#6}}%
    {\@dblarg{\@sect{#1}{#2}{#3}{#4}{#5}{#6}}}}
\def\@sect#1#2#3#4#5#6[#7]#8{%
  \ifnum #2>\c@secnumdepth
    \let\@svsec\@empty
  \else
    \refstepcounter{#1}%
    \protected@edef\@svsec{\@seccntformat{#1}\relax}%
  \fi
  \@tempskipa #5\relax
  \ifdim \@tempskipa<\z@
    \def\@svsechd{%
      #6{\hskip #3\relax
      \@svsec #8}%
      \csname #1mark\endcsname{#7}%
      \addcontentsline{toc}{#1}{%
        \ifnum #2>\c@secnumdepth \else
          \protect\numberline{\csname the#1\endcsname}%
        \fi
        #7}}% 目次にフルネームを載せるなら #8
  \else
    \begingroup
      \interlinepenalty \@M % 下から移動
      #6{%
        \@hangfrom{\hskip #3\relax\@svsec}%
        #8\@@par}%
    \endgroup
    \csname #1mark\endcsname{#7}%
    \addcontentsline{toc}{#1}{%
      \ifnum #2>\c@secnumdepth \else
        \protect\numberline{\csname the#1\endcsname}%
      \fi
      #7}% 目次にフルネームを載せるならここは #8
  \fi
  \@xsect{#5}}
\def\@xsect#1{%
  \@tempskipa #1\relax
  \ifdim \@tempskipa<\z@
    \@nobreakfalse
    \global\@noskipsectrue
    \everypar{%
      \if@noskipsec
        \global\@noskipsecfalse
       {\setbox\z@\lastbox}%
        \clubpenalty\@M
        \begingroup \@svsechd \endgroup
        \unskip
        \@tempskipa #1\relax
        \hskip -\@tempskipa
      \else
        \clubpenalty \@clubpenalty
      \fi}%
  \else
    \par \nobreak
    \vskip \@tempskipa
    \@afterheading
  \fi
  \par  % 2000-12-18
  \ignorespaces}
\setcounter{secnumdepth}{3}
\newcounter{part}
\newcounter{chapter}
\newcounter{section}[chapter]
\newcounter{bibsection}[bibsection]
\newcounter{subsection}[section]
\newcounter{subsubsection}[subsection]
\newcounter{paragraph}[subsubsection]
\newcounter{subparagraph}[paragraph]
\renewcommand\thefootnote{\arabic{footnote})} %脚注番号 x)形式
\renewcommand{\thepart}{\@Roman\c@part}
\renewcommand{\thesection}{\textbf{\@arabic\c@section.}}
\renewcommand{\thesubsection}{\thesection\textbf{\@arabic\c@subsection.}}
\renewcommand{\thesubsubsection}{%
   \thesubsection\textbf{\@arabic\c@subsubsection.}}
\renewcommand{\theparagraph}{%
   \thesubsubsection.\@arabic\c@paragraph}
\renewcommand{\thesubparagraph}{%
   \theparagraph.\@arabic\c@subparagraph}
\newcommand{\@secapp}{}
\newcommand\part{%
  \if@noskipsec \leavevmode \fi
  \par
  \addvspace{4ex}%
  \secdef\@part\@spart}
\def\@part[#1]#2{%
  \ifnum \c@secnumdepth >\m@ne
    \refstepcounter{part}%
    \addcontentsline{toc}{part}{%
      \prepartname\thepart\postpartname\hspace{2exm}#1}%
  \else
    \addcontentsline{toc}{part}{#1}%
  \fi
  \markboth{}{}%
  {\parindent\z@
    \raggedright
    \interlinepenalty \@M
    \normalfont
    \ifnum \c@secnumdepth >\m@ne
      \Large\headfont\prepartname\thepart\postpartname
      \par\nobreak
    \fi
    \huge \headfont #2%
    \markboth{}{}\par}%
  \nobreak
  \vskip 3ex
  \@afterheading}
\def\@spart#1{{%
    \parindent \z@ \raggedright
    \interlinepenalty \@M
    \normalfont
    \huge \headfont #1\par}%
  \nobreak
  \vskip 3ex
  \@afterheading}%
\newcommand{\section}{%
\ifx\title\relax\else\maketitle\fi
    \vspace{1\baselineskip}\@startsection{section}{1}{\z@}%
    {\z@}{1\baselineskip}%
    {\normalsize\normalfont\@secapp\textbf}}%
\newcommand{\subsection}{%
\ifnum\c@subsection=0\relax\def\av@beskip{1\baselineskip}\else\def\av@beskip{\z@}\fi%
\@startsection{subsection}{2}{\z@}%
    {\av@beskip}{\z@}%
    {\noindent\normalfont\normalsize\textbf}}%
\newcommand{\bibsection}{%
  \ifx\title\relax\else\maketitle\fi
\dimen0=\pagegoal \advance\dimen0 by -\pagetotal%
\ifdim\pagetotal>\pagegoal\relax\def\av@beskip{1\baselineskip}\else%
\ifdim\pagetotal<11pt\relax\def\av@beskip{\z@}\else%
\ifdim\dimen0<44pt\relax\def\av@beskip{\z@}\else\def\av@beskip{1\baselineskip}\fi\fi\fi
    \@startsection{bibsection}{1}{\z@}%
    {\av@beskip}{1\baselineskip}%
    {\normalsize\headfont\textbf}}
\newcommand{\subsubsection}{%
\dimen0=\pagegoal \advance\dimen0 by -\pagetotal%
\ifdim\pagetotal>\pagegoal\relax\def\av@beskip{0\baselineskip}\else%
\ifdim\pagetotal<11pt\relax\def\av@beskip{\z@}\else%
\ifdim\dimen0<44pt\relax\def\av@beskip{\z@}\else\def\av@beskip{0\baselineskip}\fi\fi\fi
    \@startsection{subsubsection}{3}{\z@}%
    {\av@beskip}{0.0001pt}%
    {\noindent\normalfont\normalsize\textbf}}
\newcommand{\paragraph}{
\dimen0=\pagegoal \advance\dimen0 by -\pagetotal%
\ifdim\pagetotal>\pagegoal\relax\def\av@beskip{.5\baselineskip}\else%
\ifdim\pagetotal<11pt\relax\def\av@beskip{\z@}\else%
\ifdim\dimen0<44pt\relax\def\av@beskip{\z@}\else\def\av@beskip{.5\baselineskip}\fi\fi\fi
  \@startsection{paragraph}{4}{\z@}%
    {\av@beskip}{-1em}%
    {\normalfont\normalsize\headfont}}
\newcommand{\subparagraph}{\@startsection{subparagraph}{5}{\z@}%
   {\z@}{-1em}%
   {\normalfont\normalsize\headfont}}
\setlength\leftmargini{2em}
  \setlength\leftmarginii {2em} % 2.2em
  \setlength\leftmarginiii{2em} % 1.87em
  \setlength\leftmarginiv {2em} % 1.7em
  \setlength\leftmarginv  {1em} % .5em
  \setlength\leftmarginvi {1em} % .5em
\setlength  \labelsep  {0.5em} % .5em
\setlength  \labelwidth{\leftmargini}
\addtolength\labelwidth{-\labelsep}
\setlength\partopsep{\z@} % {2\p@ \@plus 1\p@ \@minus 1\p@}
\@beginparpenalty -\@lowpenalty
\@endparpenalty   -\@lowpenalty
\@itempenalty     -\@lowpenalty
\def\@listi{\leftmargin\leftmargini
  \parsep \z@
%  \topsep 0.5\baselineskip \@plus 0.2\baselineskip \@minus 0.1\baselineskip
  \topsep 0.25\baselineskip \@plus 0.25\baselineskip \@minus 0.1\baselineskip
  \itemsep \z@ \relax}
\let\@listI\@listi
\@listi
\def\@listii{\leftmargin\leftmarginii
  \labelwidth\leftmarginii \advance\labelwidth-\labelsep
  \topsep \z@
  \parsep \z@
  \itemsep\parsep}
\def\@listiii{\leftmargin\leftmarginiii
  \labelwidth\leftmarginiii \advance\labelwidth-\labelsep
  \topsep \z@
  \parsep \z@
  \itemsep\parsep}
\def\@listiv {\leftmargin\leftmarginiv
              \labelwidth\leftmarginiv
              \advance\labelwidth-\labelsep}
\def\@listv  {\leftmargin\leftmarginv
              \labelwidth\leftmarginv
              \advance\labelwidth-\labelsep}
\def\@listvi {\leftmargin\leftmarginvi
              \labelwidth\leftmarginvi
              \advance\labelwidth-\labelsep}
\renewcommand{\theenumi}{\@arabic\c@enumi}
\renewcommand{\theenumii}{\@alph\c@enumii}
\renewcommand{\theenumiii}{\@roman\c@enumiii}
\renewcommand{\theenumiv}{\@Alph\c@enumiv}
\newcommand{\labelenumi}{\theenumi.}
\newcommand{\labelenumii}{(\theenumii)}
\newcommand{\labelenumiii}{\theenumiii.}
\newcommand{\labelenumiv}{\theenumiv.}
\renewcommand{\p@enumii}{\theenumi}
\renewcommand{\p@enumiii}{\theenumi(\theenumii)}
\renewcommand{\p@enumiv}{\p@enumiii\theenumiii}
\newcommand\labelitemi{\textbullet}
\newcommand\labelitemii{\normalfont\bfseries \textendash}
\newcommand\labelitemiii{\textasteriskcentered}
\newcommand\labelitemiv{\textperiodcentered}
\newenvironment{description}{%
  \list{}{%
    \labelwidth=\leftmargin
    \labelsep=1em
    \advance \labelwidth by -\labelsep
    \let \makelabel=\descriptionlabel}}{\endlist}
\newcommand*\descriptionlabel[1]{\normalfont\headfont #1\hfil}
\newbox\@abstractbox
\newenvironment{abstract}{%
  \global\setbox\@abstractbox\hbox\bgroup
  \begin{minipage}[u]{\abstractboxwidth}\par
    \usefont{T1}{txr}{m}{n}\small\selectfont\parindent4ex}%
  {\end{minipage}\egroup}
\newenvironment{verse}{%
  \let \\=\@centercr
  \list{}{%
    \itemsep \z@
    \itemindent -1.5em
    \listparindent\itemindent
    \rightmargin \z@
    \advance\leftmargin 1.5em}%
  \item\relax}{\endlist}
\newenvironment{quotation}{%
  \list{}{%
    \listparindent\parindent
    \itemindent\listparindent
    \rightmargin \z@}%
  \item\relax}{\endlist}
\newenvironment{quote}%
  {\list{}{\rightmargin\z@}\item\relax}{\endlist}
\newenvironment{titlepage}{%
    \if@twocolumn
      \@restonecoltrue\onecolumn
    \else
      \@restonecolfalse\newpage
    \fi
    \thispagestyle{empty}%
    \setcounter{page}\@ne
  }%
  {\if@restonecol\twocolumn \else \newpage \fi
    \if@twoside\else
      \setcounter{page}\@ne
    \fi}
\newcommand{\appendix}{\par
  \setcounter{section}{0}%
  \setcounter{subsection}{0}%
  \gdef\@secapp{\appendixname}%
  \gdef\thesection{\@Alph\c@section}}
\setlength\arraycolsep{5\p@}
\setlength\tabcolsep{6\p@}
\setlength\arrayrulewidth{.4\p@}
\setlength\doublerulesep{2\p@}
\setlength\tabbingsep{\labelsep}
\skip\@mpfootins = \skip\footins
\setlength\fboxsep{3\p@}
\setlength\fboxrule{.4\p@}
\@addtoreset{equation}{chapter}
\renewcommand\theequation
  {\ifnum \c@chapter>\z@ \thechapter.\fi \@arabic\c@equation}
\def\eqnarray{%
   \stepcounter{equation}%
   \def\@currentlabel{\p@equation\theequation}%
   \global\@eqnswtrue
   \m@th
   \global\@eqcnt\z@
   \tabskip\@centering
   \let\\\@eqncr
   $$\everycr{}\halign to\displaywidth\bgroup
       \hskip\@centering$\displaystyle\tabskip\z@skip{##}$\@eqnsel
      &\global\@eqcnt\@ne \hfil$\displaystyle{{}##{}}$\hfil
      &\global\@eqcnt\tw@ $\displaystyle{##}$\hfil\tabskip\@centering
      &\global\@eqcnt\thr@@ \hb@xt@\z@\bgroup\hss##\egroup
         \tabskip\z@skip
      \cr}
\newcounter{figure}[chapter]
\renewcommand \thefigure
     {\ifnum \c@chapter>\z@ \thechapter.\fi \@arabic\c@figure}
\def\fps@figure{tbp}
\def\ftype@figure{1}
\def\ext@figure{lof}
\def\fnum@figure{\figurename\nobreak\thefigure.}
\newenvironment{figure}%
               {\@float{figure}}%
               {\end@float}
\newenvironment{figure*}%
               {\@dblfloat{figure}}%
               {\end@dblfloat}
\newcounter{table}[chapter]
\renewcommand \thetable
     {\ifnum \c@chapter>\z@ \thechapter.\fi \@arabic\c@table}
\def\fps@table{tbp}
\def\ftype@table{2}
\def\ext@table{lot}
\def\fnum@table{\tablename\nobreak\thetable.}
\newenvironment{table}%
               {\@float{table}}%
               {\end@float}
\newenvironment{table*}%
               {\@dblfloat{table}}%
               {\end@dblfloat}
\newlength\abovecaptionskip
\newlength\belowcaptionskip
%\setlength\abovecaptionskip{5\p@} % 元: 10\p@
%\setlength\belowcaptionskip{0\p@}
\setlength\abovecaptionskip{1\p@} % 元: 10\p@
\setlength\belowcaptionskip{1\p@}
\long\def\@makecaption#1#2{{\footnotesize
\iftjsass
  \advance\leftskip3mm
  \advance\rightskip3mm
\fi
  \vskip\abovecaptionskip
  \sbox\@tempboxa{#1\hspace{1em}#2}%
  \ifdim \wd\@tempboxa >\hsize
  	\iftjsass \hskip-3mm #1\hspace{1em}#2\par
  \else
    #1\hspace{1em}#2\par
  \fi
  \else
    \global \@minipagefalse
    \hb@xt@\hsize{\hfil\box\@tempboxa\hfil}%
  \fi
  \vskip\belowcaptionskip}}
  \newlength\captionwidth \captionwidth=64mm
%
\def\hangcaption{\@ifstar{\captionwidth=100mm \refstepcounter\@captype\@dblarg{\@hangcaption\@captype}}{\refstepcounter\@captype\@dblarg{\@hangcaption\@captype}}}
\long\def\@hangcaption#1[#2]#3{%
%  \baselineskip 14.5\p@
  \addcontentsline{\csname ext@#1\endcsname}{#1}%
    {\protect\numberline{\csname the#1\endcsname}{\ignorespaces #2}}%
  \par
  \begingroup
    \@parboxrestore
    \setbox\@tempboxa\hbox{\csname fnum@#1\endcsname: #3}%
    \ifdim \wd\@tempboxa > \captionwidth
      \begingroup
        \setbox\@tempboxa\hbox{\csname fnum@#1\endcsname: }%
        \advance \captionwidth by -\wd\@tempboxa
        \@makecaption{\csname fnum@#1\endcsname}%
            {\ignorespaces{\protect\parbox[t]{\captionwidth}{\leavevmode#3}}}%
      \endgroup
    \else
      \begingroup
        \@makecaption{\csname fnum@#1\endcsname}{\ignorespaces #3}%
      \endgroup
    \fi
    \par\vspace{\belowcaptionskip}%
  \endgroup
  \captionwidth=64mm}
%
\DeclareOldFontCommand{\mc}{\normalfont\rmfamily}{\mathmc}
\DeclareOldFontCommand{\gt}{\normalfont\sffamily}{\mathgt}
\DeclareOldFontCommand{\rm}{\normalfont\rmfamily}{\mathrm}
\DeclareOldFontCommand{\sf}{\normalfont\sffamily}{\mathsf}
\DeclareOldFontCommand{\tt}{\normalfont\ttfamily}{\mathtt}
\DeclareOldFontCommand{\bf}{\normalfont\bfseries}{\mathbf}
\DeclareOldFontCommand{\it}{\normalfont\itshape}{\mathit}
\DeclareOldFontCommand{\sl}{\normalfont\slshape}{\@nomath\sl}
\DeclareOldFontCommand{\sc}{\normalfont\scshape}{\@nomath\sc}
\DeclareRobustCommand*{\cal}{\@fontswitch\relax\mathcal}
\DeclareRobustCommand*{\mit}{\@fontswitch\relax\mathnormal}
\newcommand\@pnumwidth{1.55em}
\newcommand\@tocrmarg{2.55em}
\newcommand\@dotsep{4.5}
\setcounter{tocdepth}{2}
\newcommand{\tableofcontents}{%
  \section*{\contentsname
    \@mkboth{\contentsname}{\contentsname}}%
  \@starttoc{toc}%
}
\newcommand*{\l@part}[2]{%
  \ifnum \c@tocdepth >-2\relax
    \addpenalty\@secpenalty
    \addvspace{2.25em \@plus\p@}%
    \begingroup
      \parindent \z@
      \rightskip \@tocrmarg
      \parfillskip -\@pnumwidth
      {\leavevmode
        \large \headfont
        \setlength\@lnumwidth{4em}%
        #1\hfil \hb@xt@\@pnumwidth{\hss #2}}\par
      \nobreak
    \endgroup
  \fi}
\newcommand*{\l@section}[2]{%
  \ifnum \c@tocdepth >\z@
    \addpenalty{\@secpenalty}%
    \addvspace{1.0em \@plus\p@}%
    \begingroup
      \parindent\z@
      \rightskip\@tocrmarg
      \parfillskip-\rightskip
      \leavevmode\headfont
      \setlength\@lnumwidth{1.5em}%
      \advance\leftskip\@lnumwidth \hskip-\leftskip
      #1\nobreak\hfil\nobreak\hbox to\@pnumwidth{\hss#2}\par
    \endgroup
  \fi}
\newcommand*{\l@subsection}   {\@dottedtocline{2}{1.5em}{2.3em}}
\newcommand*{\l@subsubsection}{\@dottedtocline{3}{3.8em}{3.2em}}
\newcommand*{\l@paragraph}    {\@dottedtocline{4}{7.0em}{4.1em}}
\newcommand*{\l@subparagraph} {\@dottedtocline{5}{10em}{5em}}
\newdimen\@lnumwidth
\def\numberline#1{\hb@xt@\@lnumwidth{#1\hfil}\hspace{0pt}}
\def\@dottedtocline#1#2#3#4#5{\ifnum #1>\c@tocdepth \else
  \vskip \z@ \@plus.2\p@
  {\leftskip #2\relax \rightskip \@tocrmarg \parfillskip -\rightskip
    \parindent #2\relax\@afterindenttrue
   \interlinepenalty\@M
   \leavevmode
   \@lnumwidth #3\relax
   \advance\leftskip \@lnumwidth \null\nobreak\hskip -\leftskip
    {#4}\nobreak
    \leaders\hbox{$\m@th \mkern \@dotsep mu\hbox{.}\mkern \@dotsep
       mu$}\hfill \nobreak\hb@xt@\@pnumwidth{%
         \hfil\normalfont \normalcolor #5}\par}\fi}
\newcommand{\listoffigures}{%
    \section*{\listfigurename
  \@mkboth{\listfigurename}{\listfigurename}}%
  \@starttoc{lof}%
}
\newcommand*{\l@figure}{\@dottedtocline{1}{1.5em}{2.3em}}
\newcommand{\listoftables}{%
    \section*{\listtablename
  \@mkboth{\listtablename}{\listtablename}}%
  \@starttoc{lot}%
}
\let\l@table\l@figure
\newdimen\bibindent
\setlength\bibindent{1.8em}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\def\@biblabel#1{#1)}%

\newenvironment{thebibliography}[1]{%
  \balance
  \global\let\@secapp\relax%
  \bibsection*{\centering\normalfont\textbf\refname\@mkboth{\refname}{\refname}}%
  \list{\bibfontsize\@biblabel{\@arabic\c@enumiv}}%
        {\settowidth\labelwidth{\@biblabel{#1}}%
         \setlength\labelsep{2mm}%
         \leftmargin\labelwidth
         \advance\leftmargin\labelsep
         \@openbib@code
         \usecounter{enumiv}%
         \let\p@enumiv\@empty
         \renewcommand\theenumiv{\@arabic\c@enumiv}%
   \bibfontsize
   \sloppy
   \clubpenalty4000
   \@clubpenalty\clubpenalty
   \widowpenalty4000%
   \sfcode`\.\@m}}
  {\def\@noitemerr
    {\@latex@warning{Empty `thebibliography' environment}}%
   \endlist}
\newcommand{\newblock}{\hskip .11em\@plus.33em\@minus.07em}
\let\@openbib@code\@empty

\renewcommand{\footnoterule}{%
  \kern-3\p@
  \hrule width 1.0\columnwidth
  \kern 4\p@}

\def\@item[#1]{%
  \if@noparitem
    \@donoparitem
  \else
    \if@inlabel
      \indent \par
    \fi
    \ifhmode
      \unskip\unskip \par
    \fi
    \if@newlist
      \if@nobreak
        \@nbitem
      \else
        \addpenalty\@beginparpenalty
        \addvspace\@topsep
        \addvspace{-\parskip}%
      \fi
    \else
      \addpenalty\@itempenalty
      \addvspace\itemsep
    \fi
    \global\@inlabeltrue
  \fi
  \everypar{%
    \@minipagefalse
    \global\@newlistfalse
    \if@inlabel
      \global\@inlabelfalse
      {\setbox\z@\lastbox
       \ifvoid\z@
         \kern-\itemindent
       \fi}%
      \box\@labels
      \penalty\z@
    \fi
    \if@nobreak
      \@nobreakfalse
      \clubpenalty \@M
    \else
      \clubpenalty \@clubpenalty
    \fi}%
  \if@noitemarg
    \@noitemargfalse
    \if@nmbrlist
      \refstepcounter\@listctr
    \fi
  \fi
  \sbox\@tempboxa{\makelabel{#1}}%
  \global\setbox\@labels\hbox{%
    \unhbox\@labels
    \hskip \itemindent
    \hskip -\labelwidth
    \hskip -\labelsep
    \ifdim \wd\@tempboxa >\labelwidth
      \box\@tempboxa
    \else
      \hbox to\labelwidth {\unhbox\@tempboxa}%
    \fi
    \hskip \labelsep}%
  \ignorespaces}
\def\@afterheading{%
  \@nobreaktrue
  \everypar{%
    \if@nobreak
      \@nobreakfalse
      \clubpenalty \@M
      \if@afterindent \else
        {\setbox\z@\lastbox}%
      \fi
    \else
      \clubpenalty \@clubpenalty
    \fi}}
\def\@gnewline #1{%
  \ifvmode
    \@nolnerr
  \else
    \unskip \reserved@e {\reserved@f#1}\nobreak \hfil \break
  \fi}
\def\txr{txr}
\DeclareRobustCommand{\TeX}{%
  \ifx\f@family\cmr\cmrTeX
  \else
    \ifx\f@family\ptm\ptmTeX
    \else
      \ifx\f@family\txr\ptmTeX
      \else
        \ifx\f@family\pnc\pncTeX
        \else\sfTeX
        \fi
      \fi
    \fi
  \fi}
\DeclareRobustCommand{\LaTeX}{%
  \ifx\f@family\cmr\cmrLaTeX
  \else
    \ifx\f@family\ptm\ptmLaTeX
    \else
      \ifx\f@family\txr\ptmLaTeX
      \else
        \ifx\f@family\pnc\pncLaTeX
        \else\sfLaTeX
        \fi
      \fi
    \fi
  \fi}
\DeclareRobustCommand{\LaTeXe}{$\mbox{%
  \if b\expandafter\@car\f@series\@nil\boldmath\fi
  \LaTeX\kern.15em2\raisebox{-.37ex}{$\textstyle\varepsilon$}}$}
\def\pTeX{p\kern-.05em\TeX}
\def\pLaTeX{p\LaTeX}
\def\pLaTeXe{p\LaTeXe}
\def\AmSTeX{\protect\AmS-\protect\TeX{}}
%
\newcommand{\figurename}{Fig.~}
\newcommand{\tablename}{Table~}
\newcommand{\refname}{References}
\newcommand{\bibname}{References}
%
\hyphenation{ado-be post-script ghost-script phe-nom-e-no-log-i-cal man-u-script}
%必要なパッケージの読み込み
\def\@eqnnum{(\theequation)}
\def\tagform@#1{\maketag@@@{(\ignorespaces#1\unskip\@@italiccorr)}}
%-------------------------------------------------------------------
% In order to balance the columns on a page, \balance must be given
% somewhere within the first column. To turn off the feature, give
% \nobalance. One has to look at the unbalanced text first to decide
% where best to place \balance.
%-------------------------------------------------------------------
\newcommand{\@BAlancecol}{\if@twocolumn
  \setbox0=\vbox{\unvbox\@outputbox} \@tempdima=\ht0
  \advance\@tempdima by \topskip \advance\@tempdima
     by -\baselineskip \divide\@tempdima by 2
     \splittopskip=\topskip
  {\vbadness=\@M \loop \global\setbox3=\copy0
   \global\setbox1=\vsplit3 to \@tempdima
   \ifdim\ht3>\@tempdima \global\advance\@tempdima by 1pt \repeat}
   \setbox\@leftcolumn=\vbox to \@tempdima{\unvbox1\vfil}
   \setbox\@outputbox=\vbox to \@tempdima
     {\dimen2=\dp3\unvbox3\kern-\dimen2
      \vfil}
  \fi}
\newif\if@BAlanceone
\global\@BAlanceonefalse
\newdimen\oldvsize
\newcommand{\@BAdblcol}{\if@firstcolumn
       \unvbox\@outputbox \penalty\outputpenalty
       \global\oldvsize=\@colht \global\multiply \@colht by 2
       \global\@BAlanceonetrue
       \global\@firstcolumnfalse
  \else \global\@firstcolumntrue
       \if@BAlanceone
       \global\@BAlanceonefalse\@BAlancecol
       \global\@colht=\oldvsize \else
       \PackageWarningNoLine{balance}
          {You have called \protect\balance\space
             in second column\MessageBreak
           Columns might not be balanced}\fi
     \setbox\@outputbox\vbox to \@colht{\hbox to\textwidth
     {\hbox to\columnwidth {\box\@leftcolumn \hss}\hfil
      \vrule width\columnseprule\hfil \hbox to\columnwidth
      {\box\@outputbox \hss}}\vfil}\@combinedblfloats
     \@outputpage \begingroup \@dblfloatplacement
     \@startdblcolumn \@whilesw\if@fcolmade \fi
     {\@outputpage\@startdblcolumn}\endgroup
  \fi}
\newcommand{\@BAcleardblpage}{\clearpage\if@twoside \ifodd\c@page\else
  \hbox{}\newpage\fi\fi}
\newcommand{\@@cleardblpage}{}
\let\@@cleardblpage=\cleardoublepage

\newcommand{\@@utputdblcol}{}
\let\@@utputdblcol=\@outputdblcol
\newcommand{\balance}{\global\let\@outputdblcol=\@BAdblcol
  \global\let\cleardoublepage=\@BAcleardblpage}
\newcommand{\nobalance}{\global\let\@outputdblcol=\@@utputdblcol
  \global\let\cleardoublepage=\@@cleardblpage}
%% 
%% <<<<< End of generated file <<<<<<
%%
%% End of file `balance.sty'.

\pagestyle{headings}
\pagenumbering{arabic}
\twocolumn
\sloppy
\flushbottom
\endinput

\titleheadertrue
%%
%% End of file `tjsass.cls'.

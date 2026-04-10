%%
%% This is file `aiaa-tc.cls',
%% generated with the docstrip utility.
%%
%% The original source files were:
%%
%% aiaa.dtx  (with options: `class')
%% 
%% Welcome to the AIAA LaTeX package.
%% 
%% For further details and support, read the Users Manual, aiaa.pdf.
%% 
%% This software is released under the terms of the LaTeX Project Public
%% License.  Copyright (C) 2004 by Bil Kleb, Bill Wood, and Erich Knausenberger.
%% 
%% Description: LaTeX package for AIAA technical conference papers.
%% Keywords: LaTeX, class, AIAA, BibTeX, bibliographic-style
%% Author: Bil Kleb, Bill Wood, Erich Knausenberger
%% Maintainer: same
%% Version: 3.6 <23 May 2004>
%%
%% Run (PDF)LaTeX on `aiaa.dtx' to obtain a Users Manual and code
%% documentation.  Note: to get detailed code documentation you need to
%% comment out the \OnlyDescription line in `aiaa.dtx'.
%%
%% See `aiaa.ins' and the Users Manual for installation instructions.
%%
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
%%
%%  `aiaa-tc.cls' - A LaTeX class to create AIAA technical conference papers.
%%
%%  The basic approach is to load the LaTeX article class and various
%%  standard LaTeX packages, and then make modifications and extensions to
%%  conform to AIAA's guidelines.
%%
%%  To use, simply select this class file in your LaTeX document.  For example,
%%
%%    \documentclass{aiaa-tc}
%%
%%  Class options:
%%
%%   handcarry - used for hand carrying a paper to a 'loose papers'
%%               conference.  It adds the conference name to the
%%               top of the first page, paper number to the bottom
%%               of each page, and a copyright notice on the bottom
%%               of the first
%%
%%   submit - produces output typically required for journal submission,
%%            i.e., 12pt font and 'double spacing'.
%%
%%  plus those accepted by the standard article class such as 'draft'.
%%
%%  Class-specific commands:
%%
%%   \thanksibid{} - Reuses \thanks footnote symbol from previous
%%                   author specified by position in \author command.
%%                   For example, `someone\thanksibid{2}' will reuse the
%%                   footnote symbol from the second author for 'someone'
%%                   without producing a repeated footnote at the bottom.
%%
%%   \AIAApapernumber{} - For use with 'handcarry' option.
%%    \AIAAconference{} - Ditto.
%%     \AIAAcopyright{} - Ditto.  Must be filled with one of the
%%                        various copyright types: \AIAAcopyrightA{year},
%%                        \AIAAcopyrightB{year}{holder}, \AIAAcopyrightC,
%%                        \AIAAcopyrightD{year}, or \AIAAcopyrightDAPL{year}.
%%
%%   \AIAAsubmitinfo{} - Footer information used by 'submit' option.
%%
%%  For support, further details, and a documented version of this class,
%%  please read the Users Manual, aiaa.pdf, and the templates that come
%%  with the AIAA LaTeX distribution.
%%
%%  Wish list:
%%
%%  o fine tune hyperref behavior, especially footnote behavior
%%
%%  o an \appendix command to switch section styles, counters, etc.
%%
%% o leave space for copyright notice so that handcarry option does
%%    not alter pagination.
%%
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{aiaa-tc}[2004/04/03 v3.5 AIAA technical conference paper]
\newif\if@aiaa@handcarry
\newif\if@aiaa@submit
\@aiaa@handcarryfalse
\@aiaa@submitfalse
\DeclareOption{handcarry}{\@aiaa@handcarrytrue\@aiaa@submitfalse}
\DeclareOption{submit}{\@aiaa@submittrue\@aiaa@handcarryfalse}
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}%
                \typeout{Unknown option: passing ``\CurrentOption"
                         option on to the standard LaTeX article class.}}
\ProcessOptions
\if@aiaa@submit
 \LoadClass[letterpaper,12pt]{article}
\else
 \LoadClass[letterpaper,10pt]{article}
\fi
\RequirePackage[margin=1in,top=0.75in,headheight=\baselineskip]%
               {geometry}% page layout
\RequirePackage{graphicx}% images
\RequirePackage{array}%    extra row height
\RequirePackage{overcite}% superscript, sorted, compressed citation commands
\RequirePackage{lastpage}% to get reference for last page number
\RequirePackage{fancyhdr}% simple control of headers and footers
\if@aiaa@submit
  \RequirePackage{setspace}% control line spacing
\fi
\newcommand*{\AIAApapernumber}[1]{\def\aiaa@papernumber{#1}}\AIAApapernumber{}
\newcommand*{\AIAAconference}[1]{\def\aiaa@conference{#1}}\AIAAconference{}
\newcommand*{\AIAAcopyright}[1]{\def\aiaa@copyright{#1}}\AIAAcopyright{}
\newcommand*{\AIAAsubmitinfo}[1]{\def\aiaa@submitinfo{#1}}\AIAAsubmitinfo{}
\setlength{\intextsep}{0.7\baselineskip \@plus 2\p@ \@minus 1\p@}
\def\fps@figure{htbp}
\def\fps@table{htbp}
\newcommand{\AIAAcopyrightA}[1]{Copyright \copyright\ #1 by the
  American Institute of Aeronautics and Astronautics, Inc. All
  rights reserved.}% rev 01/01/2002
\newcommand{\AIAAcopyrightB}[2]{Copyright \copyright\ #1 by
  #2. Published by the American Institute of Aeronautics
  and Astronautics, Inc.\ with permission.}% rev 01/01/2002
\newcommand{\AIAAcopyrightC}{This material is declared a work of the U.S.
  Government and is not subject to copyright protection in the
  United States.}% rev 01/01/2002
\newcommand{\AIAAcopyrightD}[1]{Copyright \copyright\ #1 by the American
  Institute of Aeronautics and Astronautics, Inc.  The U.S. Government
  has a royalty-free license to exercise all rights under the copyright
  claimed herein for Governmental purposes. All other rights are
  reserved by the copyright owner.}% rev 01/01/2002
\newcommand{\AIAAcopyrightDAPL}[1]{Copyright \copyright\ #1 by the American
  Institute of Aeronautics and Astronautics, Inc.  Under the copyright
  claimed herein, the U.S. Government has a royalty-free license to
  exercise all rights for Governmental purposes. All other rights are
  reserved by the copyright owner.}% rev 01/01/2002
\newcommand\aiaa@makecopyright{%
  \begingroup
   \renewcommand\thefootnote{}
   \if@aiaa@handcarry\footnotetext{\aiaa@copyright}\fi
  \endgroup
}
\newcounter{aiaa@cthanks}
\newcounter{aiaa@cfootnote}
\newcommand{\thanksibid}[1]{%
  \begingroup
    \setcounter{aiaa@cfootnote}{\value{footnote}}% store original
    \setcounter{aiaa@cthanks}{#1}%
    \renewcommand\thefootnote{\fnsymbol{aiaa@cthanks}}%
    \footnotemark%
    \setcounter{footnote}{\value{aiaa@cfootnote}}% reset to original
  \endgroup
}
\renewcommand\maketitle{\par
  \begingroup
    \renewcommand\thefootnote{\@fnsymbol\c@footnote}%
    \def\@makefnmark{\rlap{\@textsuperscript{\normalfont\@thefnmark}}}%
    \long\def\@makefntext##1{\parindent 1em\noindent
            \hb@xt@1.8em{%
                \hss\@textsuperscript{\normalfont\@thefnmark}}##1}%
    \newpage
    \global\@topnum\z@   % Prevents figures from going at top of page.
    \@maketitle
    \@thanks
  \endgroup
  \setcounter{footnote}{0}%
  \renewcommand\thefootnote{\@alph\c@footnote}% change to alpha style footnote
  \aiaa@makecopyright
  \global\let\thanks\relax
  \global\let\thanksibid\relax
  \global\let\maketitle\relax
  \global\let\@maketitle\relax
  \global\let\@thanks\@empty
  \global\let\@author\@empty
  \global\let\@date\@empty
  \global\let\@title\@empty
  \global\let\title\relax
  \global\let\author\relax
  \global\let\date\relax
  \global\let\and\relax
}
\def\@maketitle{%
  \newpage
  \null
  \begin{center}%
  \if@aiaa@handcarry
    {\sffamily\scriptsize\aiaa@conference}
    \vskip1em
  \else
    \vskip\baselineskip
    \vskip1em
  \fi
  \let \footnote \thanks
    {\LARGE\bfseries \@title \par}%
    \vskip 1.5em%
    {\large
      \lineskip .5em%
      \begin{tabular}[t]{c}%
        \@author
      \end{tabular}\par}%
  \end{center}%
  \par
  \vskip 1.5ex}
\setlength\hfuzz{2pt}
\fancyhf{}% clear defaults
\renewcommand{\headrulewidth}{0pt}% turn off default headrule
\cfoot{\footnotesize\normalfont
       \thepage\ of \pageref{LastPage}\\
       \rule[.2\baselineskip]{0.5in}{0.2pt}\\
       \if@aiaa@submit
        \aiaa@submitinfo
       \else
        American Institute of Aeronautics and Astronautics
        \ifx\aiaa@papernumber\@empty
          \relax
        \else
          \if@aiaa@handcarry Paper \aiaa@papernumber\fi
        \fi
       \fi}
\pagestyle{fancy}
\setkeys{Gin}{width=\linewidth,totalheight=\textheight,keepaspectratio}
\setlength{\extrarowheight}{2pt}
\renewenvironment{abstract}%
  {\small\quotation\bfseries}%
  {\normalfont\endquotation\normalsize}
\long\def\@makecaption#1#2{%
  \vskip\abovecaptionskip
  \sbox\@tempboxa{\footnotesize\bfseries #1.~~#2}%
  \ifdim \wd\@tempboxa >\hsize
    \footnotesize\bfseries #1.~~#2\par
  \else
    \global \@minipagefalse
    \hb@xt@\hsize{\hfil\box\@tempboxa\hfil}%
  \fi
  \vskip\belowcaptionskip}
\setlength\belowcaptionskip{5pt plus 1pt minus 1pt}
\renewcommand\thesection      {\@Roman\c@section}
\renewcommand\thesubsection   {\@Alph\c@subsection}
\renewcommand\thesubsubsection{\@arabic\c@subsubsection}
\renewcommand\theparagraph    {\@alph\c@paragraph}
\renewcommand\thesubparagraph {\@roman\c@subparagraph}
\setcounter{secnumdepth}{3}% no counters \paragraph and below
\def\@seccntformat#1{\csname the#1\endcsname.\quad}
\renewcommand\section{\@startsection {section}{1}{\z@}%
                                     {3.5ex \@plus -1ex \@minus -.2ex}%
                                     {2.3ex \@plus.2ex}%
                                     {\center\normalfont\large\bfseries}}
\renewcommand\subsection{\@startsection{subsection}{2}{\z@}%
                                       {-3.25ex\@plus -1ex \@minus -.2ex}%
                                       {1.5ex \@plus .2ex}%
                                       {\normalfont\normalsize\bfseries}}
\renewcommand\subsubsection{\@startsection{subsubsection}{3}{\z@}%
                                       {-3.25ex\@plus -1ex \@minus -.2ex}%
                                       {1.5ex \@plus .2ex}%
                                       {\normalfont\normalsize\itshape}}
\renewcommand\paragraph{\@startsection{paragraph}{4}{\z@}%
                                      {2.25ex \@plus1ex \@minus.2ex}%
                                      {-1em}%
                                      {\normalfont\normalsize\scshape}}
\renewcommand\subparagraph{\@startsection{subparagraph}{5}{\parindent}%
                                         {2.25ex \@plus1ex \@minus .2ex}%
                                         {-1em}%
                                         {\normalfont\normalfont}}
\renewcommand{\@biblabel}[1]{$^{#1}$}
\renewenvironment{thebibliography}[1]%
     {\section*{\refname\@mkboth{\MakeUppercase\refname}%
                                 {\MakeUppercase\refname}}%
      \list{\@biblabel{\@arabic\c@enumiv}}%
           {\setlength{\leftmargin}{0pt}%
            \settowidth{\labelwidth}{\@biblabel{#1}}%
            \setlength{\itemindent}{\parindent}%
            \advance\itemindent by \labelwidth%
            \setlength{\labelsep}{0.0em}%
            \setlength{\itemsep}{-\smallskipamount}%
            \@openbib@code%
            \usecounter{enumiv}%
            \let\p@enumiv\@empty%
            \renewcommand\theenumiv{\@arabic\c@enumiv}%
            \footnotesize%
            }
      \sloppy\clubpenalty4000\widowpenalty4000%
      \sfcode`\.\@m}
      {\def\@noitemerr
        {\@latex@warning{Empty `thebibliography' environment}}%
      \endlist}
\AtBeginDocument{%
 \if@aiaa@submit\onehalfspacing\fi
 \@ifundefined{TPT@hsize}{}{%
  \def\tablenotes{%
    \TPT@hsize
    \list{}%
         {\footnotesize\topsep4pt \partopsep\z@skip
          \itemsep2pt \parsep\z@ \itemindent\z@ \leftmargin1.5em
          \def\makelabel##1{\tnote{##1}\hfil}\labelwidth0.5em\labelsep1pt%
          \relax}}
 }%
 \@ifundefined{nomgroup}{}{%
  \RequirePackage{ifthen}% for typesetting groups within nomenclature
  \setlength{\nomitemsep}{-\parsep}
  \renewcommand{\nomgroup}[1]{%
   \medskip%
   \ifthenelse{\equal{#1}{A}}%
    {\relax}%
    {\ifthenelse{\equal{#1}{G}}%
      {\item []\hspace*{-\leftmargin}%
       \emph{Symbols}\hfill\smallskip}%
      {\ifthenelse{\equal{#1}{B}}%
        {\item []\hspace*{-\leftmargin}%
         \emph{Subscripts}\hfill\smallskip}%
        {\ifthenelse{\equal{#1}{C}}%
          {\item []\hspace*{-\leftmargin}%
           \emph{Conventions}\hfill\smallskip}%
          {\ifthenelse{\equal{#1}{T}}%
            {\item []\hspace*{-\leftmargin}%
             \emph{Superscripts}\hfill\smallskip}{}%
          }%
        }%
      }%
    }%
  }% \nomgroup
 }% @ifundefined
}% \AtBeginDocument
\endinput
%%
%% End of file `aiaa-tc.cls'.

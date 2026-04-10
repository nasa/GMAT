<?xml version='1.0'?> 
<xsl:stylesheet  
    xmlns:d="http://docbook.org/ns/docbook"
    xmlns:fo="http://www.w3.org/1999/XSL/Format"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    version="1.0"> 

    <xsl:import href="../../build/contrib/docbook-xsl-ns-1.78.1/fo/docbook.xsl"/>
    
    <!-- Title page -->
    <xsl:import href="../fo.titlepage.templates.xsl"/>

    <!-- Local variables -->
    <xsl:variable name="black" select="'#000000'"/>
    <xsl:variable name="blue" select="'#0b3d91'"/>
    <xsl:variable name="blue25" select="'#c2cee3'"/>
    <xsl:variable name="gray" select="'#79797c'"/>
    <xsl:variable name="red" select="'#fc3d21'"/>
    <xsl:variable name="red25" select="'#fecec7'"/>
    <xsl:variable name="white" select="'#ffffff'"/>

    <!-- Page layout -->
    <xsl:param name="double.sided">1</xsl:param>

    <!-- Fonts -->
    <!-- other font option is Helvetica:
    <xsl:param name="body.font.family">Helvetica</xsl:param>
    -->
    <xsl:param name="body.font.family">Arial</xsl:param>
    <xsl:param name="body.font.master">11</xsl:param>
    <xsl:param name="title.font.family">Arial</xsl:param>
    <xsl:param name="monospace.font.family">Consolas, monospace</xsl:param>

    <!--
         Links
    -->
    <!-- show URLs next to external links -->
    <xsl:param name="ulink.show">0</xsl:param>
    <!-- turn links blue -->
    <xsl:attribute-set name="xref.properties">
        <xsl:attribute name="color">
            <xsl:value-of select="$blue"/>
        </xsl:attribute>
    </xsl:attribute-set>

    <!-- Titles -->
    <xsl:attribute-set name="component.title.properties">
        <xsl:attribute name="color">
            <xsl:value-of select="$blue"/>
        </xsl:attribute>
    </xsl:attribute-set>
    <xsl:attribute-set name="formal.title.properties">
        <xsl:attribute name="text-align">center</xsl:attribute>
    </xsl:attribute-set>
    <xsl:attribute-set name="section.title.properties">
        <xsl:attribute name="color">
            <xsl:value-of select="$blue"/>
        </xsl:attribute>
    </xsl:attribute-set>
    <xsl:attribute-set name="section.title.level1.properties">
        <xsl:attribute name="font-size">
            <xsl:value-of select="$body.font.master * 1.3"/>
        </xsl:attribute>
    </xsl:attribute-set>
    <xsl:attribute-set name="section.title.level2.properties">
        <xsl:attribute name="font-size">
            <xsl:value-of select="$body.font.master * 1.2"/>
        </xsl:attribute>
        <xsl:attribute name="color">
            <xsl:choose>
                <xsl:when test="ancestor-or-self::d:refentry">
                    <xsl:value-of select="$blue"/>
                </xsl:when>
                <xsl:otherwise>
                    <!--<xsl:value-of select="$gray"/>-->
                    <xsl:value-of select="$blue"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:attribute>
        <xsl:attribute name="start-indent">
            <xsl:value-of select="$body.start.indent"/>
        </xsl:attribute>
    </xsl:attribute-set>
    <xsl:attribute-set name="section.title.level3.properties">
        <xsl:attribute name="font-size">
            <xsl:value-of select="$body.font.master * 1.1"/>
        </xsl:attribute>
        <xsl:attribute name="color">
            <!--<xsl:value-of select="$gray"/>-->
            <xsl:value-of select="$blue"/>
        </xsl:attribute>
        <xsl:attribute name="start-indent">
            <xsl:value-of select="$body.start.indent"/>
        </xsl:attribute>
    </xsl:attribute-set>
    <!-- level 4 sections: blue, indented -->
    <xsl:attribute-set name="section.title.level4.properties">
        <xsl:attribute name="font-size">
            <xsl:value-of select="$body.font.master"/>
        </xsl:attribute>
        <xsl:attribute name="color">
            <xsl:value-of select="$blue"/>
        </xsl:attribute>
        <xsl:attribute name="start-indent">
            <xsl:value-of select="$body.start.indent"/>
        </xsl:attribute>
    </xsl:attribute-set>

    <!-- Sections -->
    <xsl:attribute-set name="section.level1.properties" use-attribute-sets="section.properties">
        <xsl:attribute name="break-after">
            <xsl:choose>
                <xsl:when test="ancestor::d:appendix">page</xsl:when>
                <xsl:otherwise>auto</xsl:otherwise>
            </xsl:choose>
        </xsl:attribute>
    </xsl:attribute-set>

    <!-- Auto-numering -->
    <xsl:param name="chapter.autolabel" select="0"></xsl:param>

    <!-- Program listings -->
    <xsl:param name="shade.verbatim" select="1"/>
    <xsl:attribute-set name="monospace.verbatim.properties">
        <xsl:attribute name="font-size">10</xsl:attribute>
    </xsl:attribute-set>

    <!-- Formatting for specific tags -->
    <xsl:template match="d:guibutton">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>
    <xsl:template match="d:guiicon">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>
    <xsl:template match="d:guilabel">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>
    <xsl:template match="d:guimenu">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>
    <xsl:template match="d:guimenuitem">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>
    <xsl:template match="d:guisubmenu">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>
    <xsl:template match="d:classname">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>
    <xsl:template match="d:function">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>

    <!-- Table of contents -->
    <xsl:param name="toc.section.depth">1</xsl:param>
    <xsl:param name="generate.toc">
        /appendix toc,title
        appendix  toc
        article/appendix  nop
        /article  toc,title
        book      toc,title
        <!-- /chapter  toc,title -->
        <!-- part      toc,title -->
        /preface  toc,title
        reference toc,title
        /sect1    toc
        /sect2    toc
        /sect3    toc
        /sect4    toc
        /sect5    toc
        /section  toc
        set       toc,title
    </xsl:param>
    <xsl:template match="d:section[@role = 'notintoc']" mode="toc"/>

    <!-- copied from http://sagehill.net/docbookxsl/PrintToc.html#PartToc -->
    <xsl:template name="part.titlepage.before.verso" priority="1">
        <xsl:variable name="toc.params">
            <xsl:call-template name="find.path.params">
                <xsl:with-param name="table"
                    select="normalize-space($generate.toc)"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:if test="contains($toc.params, 'toc')">
            <xsl:call-template name="division.toc">
                <xsl:with-param name="toc.context" select="."/>
            </xsl:call-template>
        </xsl:if>
    </xsl:template>
    <xsl:template name="generate.part.toc">
    </xsl:template>

    <!-- Figure/Table/Example title placement -->
    <xsl:param name="formal.title.placement">
        figure after
        example before
        equation before
        table before
        procedure before
        task before
    </xsl:param>
    <!-- switched default values for titles below the figure -->
    <xsl:attribute-set name="formal.title.properties" use-attribute-sets="normal.para.spacing">
        <xsl:attribute name="space-before.minimum">
            <xsl:choose>
                <xsl:when test="ancestor-or-self::d:figure">0.4em</xsl:when>
                <xsl:otherwise>0.8em</xsl:otherwise>
            </xsl:choose>
        </xsl:attribute>
        <xsl:attribute name="space-before.optimum">
            <xsl:choose>
                <xsl:when test="ancestor-or-self::d:figure">0.6em</xsl:when>
                <xsl:otherwise>1em</xsl:otherwise>
            </xsl:choose>
        </xsl:attribute>
        <xsl:attribute name="space-before.maximum">
            <xsl:choose>
                <xsl:when test="ancestor-or-self::d:figure">0.8em</xsl:when>
                <xsl:otherwise>1.2em</xsl:otherwise>
            </xsl:choose>
        </xsl:attribute>
        <xsl:attribute name="space-after.minimum">
            <xsl:choose>
                <xsl:when test="ancestor-or-self::d:figure">0.8em</xsl:when>
                <xsl:otherwise>0.4em</xsl:otherwise>
            </xsl:choose>
        </xsl:attribute>
        <xsl:attribute name="space-after.optimum">
            <xsl:choose>
                <xsl:when test="ancestor-or-self::d:figure">1em</xsl:when>
                <xsl:otherwise>0.6em</xsl:otherwise>
            </xsl:choose>
        </xsl:attribute>
        <xsl:attribute name="space-after.maximum">
            <xsl:choose>
                <xsl:when test="ancestor-or-self::d:figure">1.2em</xsl:when>
                <xsl:otherwise>0.8em</xsl:otherwise>
            </xsl:choose>
        </xsl:attribute>
    </xsl:attribute-set>

    <!-- Equation numbering (templates copied from
         DocBook XSL Stylesheets 1.76.1) -->
    <xsl:template match="d:equation">
        <fo:table width="100%" table-layout="fixed">
            <xsl:attribute name="id">
                <xsl:call-template name="object.id"/>
            </xsl:attribute>
            <fo:table-column column-width="90%"/>
            <fo:table-column column-width="10%"/>
            <fo:table-body start-indent="0pt" end-indent="0pt">
                <fo:table-row>
                    <fo:table-cell text-align="center">
                        <fo:block>
                            <xsl:apply-templates/>
                        </fo:block>
                    </fo:table-cell>
                    <fo:table-cell text-align="right" display-align="center">
                        <fo:block>
                            <xsl:text>(</xsl:text>
                            <xsl:apply-templates select="." mode="label.markup"/>
                            <xsl:text>)</xsl:text>
                        </fo:block>
                    </fo:table-cell>
                </fo:table-row>
            </fo:table-body>
        </fo:table>
    </xsl:template>
    <xsl:template match="d:informalequation">
        <fo:table width="100%" table-layout="fixed">
            <xsl:attribute name="id">
                <xsl:call-template name="object.id"/>
            </xsl:attribute>
            <fo:table-column column-width="90%"/>
            <fo:table-column column-width="10%"/>
            <fo:table-body start-indent="0pt" end-indent="0pt">
                <fo:table-row>
                    <fo:table-cell text-align="center">
                        <fo:block>
                            <xsl:apply-templates/>
                        </fo:block>
                    </fo:table-cell>
                    <fo:table-cell text-align="right" display-align="center">
                        <fo:block></fo:block>
                    </fo:table-cell>
                </fo:table-row>
            </fo:table-body>
        </fo:table>
    </xsl:template>

    <!-- Tables -->
    <xsl:param name="table.frame.border.style">double</xsl:param>
    <xsl:param name="table.frame.border.thickness">thick</xsl:param>
    <xsl:attribute-set name="table.properties">
        <xsl:attribute name="keep-together.within-column">always</xsl:attribute>
    </xsl:attribute-set>
    
    <!-- copied from DocBook XSL Stylesheets 1.76.1 -->
    <xsl:template name="table.cell.properties">
        <xsl:param name="bgcolor.pi" select="''"/>
        <xsl:param name="rowsep.inherit" select="1"/>
        <xsl:param name="colsep.inherit" select="1"/>
        <xsl:param name="col" select="1"/>
        <xsl:param name="valign.inherit" select="''"/>
        <xsl:param name="align.inherit" select="''"/>
        <xsl:param name="char.inherit" select="''"/>

        <xsl:attribute name="keep-together.within-column">always</xsl:attribute>

        <xsl:choose>
            <xsl:when test="ancestor::d:tgroup">
                <xsl:if test="$bgcolor.pi != ''">
                    <xsl:attribute name="background-color">
                        <xsl:value-of select="$bgcolor.pi"/>
                    </xsl:attribute>
                </xsl:if>

                <xsl:if test="$rowsep.inherit &gt; 0">
                    <xsl:call-template name="border">
                        <xsl:with-param name="side" select="'bottom'"/>
                    </xsl:call-template>
                </xsl:if>

      <xsl:if test="$colsep.inherit &gt; 0 and 
                      $col &lt; (ancestor::d:tgroup/@cols|ancestor::d:entrytbl/@cols)[last()]">
        <xsl:call-template name="border">
          <xsl:with-param name="side" select="'end'"/>
        </xsl:call-template>
      </xsl:if>

      <xsl:if test="$valign.inherit != ''">
        <xsl:attribute name="display-align">
          <xsl:choose>
            <xsl:when test="$valign.inherit='top'">before</xsl:when>
            <xsl:when test="$valign.inherit='middle'">center</xsl:when>
            <xsl:when test="$valign.inherit='bottom'">after</xsl:when>
            <xsl:otherwise>
              <xsl:message>
                <xsl:text>Unexpected valign value: </xsl:text>
                <xsl:value-of select="$valign.inherit"/>
                <xsl:text>, center used.</xsl:text>
              </xsl:message>
              <xsl:text>center</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:attribute>
      </xsl:if>

      <xsl:choose>
        <xsl:when test="$align.inherit = 'char' and $char.inherit != ''">
          <xsl:attribute name="text-align">
            <xsl:value-of select="$char.inherit"/>
          </xsl:attribute>
        </xsl:when>
        <xsl:when test="$align.inherit != ''">
          <xsl:attribute name="text-align">
            <xsl:value-of select="$align.inherit"/>
          </xsl:attribute>
        </xsl:when>
      </xsl:choose>

    </xsl:when>
    <xsl:otherwise>
      <!-- HTML table -->
      <xsl:if test="$bgcolor.pi != ''">
        <xsl:attribute name="background-color">
          <xsl:value-of select="$bgcolor.pi"/>
        </xsl:attribute>
      </xsl:if>

      <xsl:if test="$align.inherit != ''">
        <xsl:attribute name="text-align">
          <xsl:value-of select="$align.inherit"/>
        </xsl:attribute>
      </xsl:if>

      <xsl:if test="$valign.inherit != ''">
        <xsl:attribute name="display-align">
          <xsl:choose>
            <xsl:when test="$valign.inherit='top'">before</xsl:when>
            <xsl:when test="$valign.inherit='middle'">center</xsl:when>
            <xsl:when test="$valign.inherit='bottom'">after</xsl:when>
            <xsl:otherwise>
              <xsl:message>
                <xsl:text>Unexpected valign value: </xsl:text>
                <xsl:value-of select="$valign.inherit"/>
                <xsl:text>, center used.</xsl:text>
              </xsl:message>
              <xsl:text>center</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:attribute>
      </xsl:if>

      <xsl:call-template name="html.table.cell.rules"/>

    </xsl:otherwise>
  </xsl:choose>

</xsl:template>

    <!-- Cross-references -->
    <xsl:param name="xref.with.number.and.title">0</xsl:param>

    <!-- Headers/Footers -->
    <xsl:attribute-set name="header.content.properties">
        <xsl:attribute name="color">
            <xsl:value-of select="$blue"/>
        </xsl:attribute>
    </xsl:attribute-set>
    <xsl:attribute-set name="footer.content.properties">
        <xsl:attribute name="color">
            <xsl:value-of select="$blue"/>
        </xsl:attribute>
    </xsl:attribute-set>

    <xsl:template name="header.content">
        <xsl:param name="pageclass" select="''"/>
        <xsl:param name="sequence" select="''"/>
        <xsl:param name="position" select="''"/>
        <xsl:param name="gentext-key" select="''"/>

        <fo:block>
            <xsl:choose>
                <xsl:when test="$pageclass = 'titlepage'">
                    <!-- empty -->
                </xsl:when>

                <!-- inside header -->
                <xsl:when test="$double.sided != 0
                                and $sequence = 'odd'
                                and $position = 'left'">
                    <xsl:apply-templates select="." mode="titleabbrev.markup"/> 
                </xsl:when>
                <xsl:when test="$double.sided != 0
                                and $sequence = 'even'
                                and $position = 'right'">
                    <xsl:apply-templates select="." mode="titleabbrev.markup"/> 
                </xsl:when>
                <xsl:when test="$double.sided = 0
                                and $position = 'right'">
                    <xsl:apply-templates select="." mode="titleabbrev.markup"/> 
                </xsl:when>

                <!-- outside header -->
                <xsl:when test="$double.sided != 0
                                and ($sequence = 'odd' or $sequence = 'first')
                                and $position = 'right'">
                    <xsl:value-of select="ancestor-or-self::d:part/d:title"/> 
                </xsl:when>
                <xsl:when test="$double.sided != 0
                                and $sequence = 'even'
                                and $position = 'left'">
                    <xsl:value-of select="ancestor-or-self::d:part/d:title"/> 
                </xsl:when>
                <xsl:when test="$double.sided = 0
                                and $position = 'left'">
                    <xsl:value-of select="ancestor-or-self::d:part/d:title"/> 
                </xsl:when>
            </xsl:choose>
        </fo:block>
    </xsl:template>

    <!-- Lists -->
    <xsl:attribute-set name="list.item.spacing">
        <!-- defaults from compact.list.item.spacing -->
        <xsl:attribute name="space-before.optimum">0em</xsl:attribute>
        <xsl:attribute name="space-before.minimum">0em</xsl:attribute>
        <xsl:attribute name="space-before.maximum">0.2em</xsl:attribute>
    </xsl:attribute-set>
    <xsl:attribute-set name="variablelist.term.properties">
        <xsl:attribute name="font-weight">bold</xsl:attribute>
    </xsl:attribute-set>

    <!-- Admonitions -->
    <xsl:param name="admon.graphics">1</xsl:param>
    <xsl:param name="admon.graphics.extension">.svg</xsl:param>
    <xsl:attribute-set name="graphical.admonition.properties">
        <xsl:attribute name="background-color">
            <xsl:choose>
                <xsl:when test="ancestor-or-self::d:tip">
                    <xsl:value-of select="$white"/>
                </xsl:when>
                <xsl:when test="ancestor-or-self::d:note">
                    <xsl:value-of select="$blue25"/>
                </xsl:when>
                <xsl:when test="ancestor-or-self::d:important">
                    <xsl:value-of select="$white"/>
                </xsl:when>
                <xsl:when test="ancestor-or-self::d:caution">
                    <xsl:value-of select="$red25"/>
                </xsl:when>
                <xsl:when test="ancestor-or-self::d:warning">
                    <xsl:value-of select="$red25"/>
                </xsl:when>
            </xsl:choose>
        </xsl:attribute>
        <xsl:attribute name="border-style">solid</xsl:attribute>
        <xsl:attribute name="border-color">
            <xsl:value-of select="$black"/>
        </xsl:attribute>
        <xsl:attribute name="padding-right">1em</xsl:attribute>
        <xsl:attribute name="padding-bottom">1em</xsl:attribute>
        <xsl:attribute name="keep-together.within-column">always</xsl:attribute>
    </xsl:attribute-set>
    
    <!-- Verbatim environments -->
    <xsl:attribute-set name="verbatim.properties">
        <xsl:attribute name="wrap-option">wrap</xsl:attribute>
        <xsl:attribute name="keep-together.within-column">auto</xsl:attribute>
        <xsl:attribute name="background-color">transparent</xsl:attribute>
    </xsl:attribute-set>

    <!-- Reference pages -->
    <xsl:template match="d:refname"></xsl:template>
    <xsl:template match="d:refpurpose">
        <xsl:if test="node()">
            <xsl:apply-templates/>
        </xsl:if>
    </xsl:template>
    
    <xsl:attribute-set name="refentry.title.properties">
        <xsl:attribute name="font-size">
            <xsl:value-of select="$body.font.master * 1.3"/>
        </xsl:attribute>
        <xsl:attribute name="color">
            <xsl:value-of select="$blue"/>
        </xsl:attribute>
        <xsl:attribute name="start-indent">
            <xsl:value-of select="$body.start.indent"/>
        </xsl:attribute>
    </xsl:attribute-set>

    <xsl:template match="d:phrase[@role='ref_seealso']">
        <xsl:call-template name="inline.italicseq"/>
    </xsl:template>

    <xsl:template match="d:refsynopsisdiv//d:literal">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>

    <!-- Title page -->
    <xsl:template match="d:edition" mode="titlepage.mode">
        <xsl:apply-templates mode="titlepage.mode"/>
    </xsl:template>


<!-- Optional ability to set font size within programlisting which uses monospace verbatim 
copied from:  http://www.sagehill.net/docbookxsl/FittingText.html   -->
	<xsl:attribute-set name="monospace.verbatim.properties">
  <xsl:attribute name="font-size">
    <xsl:choose>
      <xsl:when test="processing-instruction('db-font-size')"><xsl:value-of
           select="processing-instruction('db-font-size')"/></xsl:when>
      <xsl:otherwise>inherit</xsl:otherwise>
    </xsl:choose>
  </xsl:attribute>
</xsl:attribute-set>

	
</xsl:stylesheet>

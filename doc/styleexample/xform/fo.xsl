<?xml version='1.0'?> 
<xsl:stylesheet  
    xmlns:d="http://docbook.org/ns/docbook"
    xmlns:fo="http://www.w3.org/1999/XSL/Format"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    version="1.0"> 

    <xsl:import href="../../build/contrib/docbook-xsl-ns/fo/docbook.xsl"/> 

    <!-- Local variables -->
    <xsl:variable name="blue" select="'#0b3d91'"/>
    <xsl:variable name="gray" select="'#79797c'"/>

    <!-- Page layout -->
    <xsl:param name="double.sided">1</xsl:param>

    <!-- Fonts -->
    <!-- other font option is Helvetica:
    <xsl:param name="body.font.family">Helvetica</xsl:param>
    -->
    <xsl:param name="body.font.family">Garamond</xsl:param>
    <xsl:param name="body.font.master">11</xsl:param>
    <xsl:param name="title.font.family">Helvetica</xsl:param>

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
            <xsl:value-of select="$gray"/>
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
            <xsl:value-of select="$gray"/>
        </xsl:attribute>
        <xsl:attribute name="start-indent">
            <xsl:value-of select="$body.start.indent"/>
        </xsl:attribute>
    </xsl:attribute-set>

    <!-- Auto-numering -->
    <xsl:param name="chapter.autolabel" select="0"></xsl:param>

    <!-- Program listings -->
    <xsl:param name="shade.verbatim" select="1"/>
    <xsl:attribute-set name="monospace.verbatim.properties">
        <xsl:attribute name="font-family">Consolas, monospace</xsl:attribute>
    </xsl:attribute-set>

    <!-- Formatting for specific tags -->
    <xsl:template match="d:guilabel">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>

    <!-- Table of contents -->
    <xsl:param name="toc.section.depth">1</xsl:param>
    <xsl:param name="generate.toc">
        /appendix toc,title
        article/appendix  nop
        /article  toc,title
        book      toc,title
        /chapter  toc,title
        part      toc,title
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
</xsl:stylesheet>

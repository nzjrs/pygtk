<?xml version="1.0"?> <!--*- mode: xml -*-->
<!DOCTYPE xsl:stylesheet [
<!ENTITY RE "&#10;">
<!ENTITY nbsp "&#160;">
]>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'
                xmlns="http://www.w3.org/TR/xhtml1/transitional"
                exclude-result-prefixes="#default">

  <xsl:import href="http://docbook.sourceforge.net/release/xsl/current/html/chunk.xsl"/>
  <xsl:include href="devhelp.xsl"/>

  <xsl:param name="toc.section.depth">1</xsl:param>
  <xsl:param name="chapter.autolabel" select="0"/>
  <xsl:param name="use.id.as.filename" select="'1'"/>
  <xsl:param name="html.ext" select="'.html'"/>
  <xsl:param name="funcsynopsis.style">ansi</xsl:param>
  <xsl:param name="refentry.generate.name" select="0"/>
  <xsl:param name="refentry.generate.title" select="1"/>

  <xsl:param name="gtkdoc.bookname" select="'pygtk-2.0'"/>

  <xsl:template match="article|book">
    <xsl:apply-imports/>

    <xsl:call-template name="generate.devhelp"/>
  </xsl:template>

  <xsl:template name="user.head.content">
    <style>
      <xsl:attribute name="type"><xsl:text>text/css</xsl:text></xsl:attribute>
      <xsl:text>
        .synopsis, .classsynopsis, .programlisting {
            background: #d6e8ff;
            padding: 4px;
        }
        .variablelist {
            background: #ffd0d0;
            padding: 4px;
        }
      </xsl:text>
    </style>
  </xsl:template>

  <!-- support for Python language for synopsises -->
  <xsl:template match="classsynopsis
                     |fieldsynopsis
                     |methodsynopsis
                     |constructorsynopsis
                     |destructorsynopsis">
    <xsl:param name="language">
      <xsl:choose>
        <xsl:when test="@language">
          <xsl:value-of select="@language"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$default-classsynopsis-language"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:param>
    <xsl:choose>
      <xsl:when test="$language='python'">
        <xsl:apply-templates select="." mode="python"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-imports/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="classsynopsis" mode="python">
    <pre class="{name(.)}">
      <xsl:text>class </xsl:text>
      <xsl:apply-templates select="ooclass[1]" mode="python"/>
      <xsl:if test="ooclass[position() &gt; 1]">
        <xsl:text>(</xsl:text>
        <xsl:apply-templates select="ooclass[position() &gt; 1]" mode="python"/>
        <xsl:text>)</xsl:text>
      </xsl:if>
      <xsl:text>:&RE;</xsl:text>

      <xsl:apply-templates select="constructorsynopsis
                                   |destructorsynopsis
                                   |fieldsynopsis
                                   |methodsynopsis
                                   |classsynopsisinfo" mode="python"/>
    </pre>
  </xsl:template>

  <xsl:template match="classsynopsisinfo" mode="python">
    <xsl:apply-templates mode="python"/>
  </xsl:template>

  <xsl:template match="ooclass|oointerface|ooexception" mode="python">
    <xsl:if test="position() &gt; 1">
      <xsl:text>, </xsl:text>
    </xsl:if>
    <span class="{name(.)}">
      <xsl:apply-templates mode="python"/>
    </span>
  </xsl:template>

  <xsl:template match="modifier" mode="python">
    <span class="{name(.)}">
      <xsl:apply-templates mode="python"/>
      <xsl:text>&nbsp;</xsl:text>
    </span>
  </xsl:template>

  <xsl:template match="classname" mode="python">
    <xsl:if test="name(preceding-sibling::*[1]) = 'classname'">
      <xsl:text>, </xsl:text>
    </xsl:if>
    <span class="{name(.)}">
      <xsl:apply-templates mode="python"/>
    </span>
  </xsl:template>

  <xsl:template match="interfacename" mode="python">
    <xsl:if test="name(preceding-sibling::*[1]) = 'interfacename'">
      <xsl:text>, </xsl:text>
    </xsl:if>
    <span class="{name(.)}">
      <xsl:apply-templates mode="python"/>
    </span>
  </xsl:template>

  <xsl:template match="exceptionname" mode="python">
    <xsl:if test="name(preceding-sibling::*[1]) = 'exceptionname'">
      <xsl:text>, </xsl:text>
    </xsl:if>
    <span class="{name(.)}">
      <xsl:apply-templates mode="python"/>
    </span>
  </xsl:template>

  <xsl:template match="fieldsynopsis" mode="python">
    <code class="{name(.)}">
      <xsl:text>&nbsp;&nbsp;&nbsp;&nbsp;</xsl:text>
      <xsl:apply-templates mode="python"/>
    </code>
    <xsl:call-template name="synop-break"/>
  </xsl:template>

  <xsl:template match="type" mode="python">
    <span class="{name(.)}">
      <xsl:apply-templates mode="python"/>
      <xsl:text>&nbsp;</xsl:text>
    </span>
  </xsl:template>

  <xsl:template match="varname" mode="python">
    <span class="{name(.)}">
      <xsl:apply-templates mode="python"/>
      <xsl:text>&nbsp;</xsl:text>
    </span>
  </xsl:template>

  <xsl:template match="initializer" mode="python">
    <span class="{name(.)}">
      <xsl:text>=</xsl:text>
      <xsl:apply-templates mode="python"/>
    </span>
  </xsl:template>

  <xsl:template match="void" mode="python">
    <span class="{name(.)}">
      <xsl:text>void&nbsp;</xsl:text>
    </span>
  </xsl:template>

  <xsl:template match="methodname" mode="python">
    <span class="{name(.)}">
      <xsl:apply-templates mode="python"/>
    </span>
  </xsl:template>

  <xsl:template match="methodparam" mode="python">
    <xsl:if test="position() &gt; 1">
      <xsl:text>, </xsl:text>
    </xsl:if>
    <span class="{name(.)}">
      <xsl:apply-templates mode="python"/>
    </span>
  </xsl:template>

  <xsl:template match="parameter" mode="python">
    <span class="{name(.)}">
      <xsl:apply-templates mode="python"/>
    </span>
  </xsl:template>

  <xsl:template mode="python"
    match="constructorsynopsis|destructorsynopsis|methodsynopsis">

    <code class="{name(.)}">
      <xsl:text>    def </xsl:text>
      <xsl:apply-templates select="methodname" mode="python"/>
      <xsl:text>(</xsl:text>
      <xsl:apply-templates select="methodparam" mode="python"/>
      <xsl:text>)</xsl:text>
    </code>
    <xsl:call-template name="synop-break"/>
  </xsl:template>

  <!-- hack -->
  <xsl:template match="link" mode="python">
    <xsl:apply-templates select="."/>
  </xsl:template>

</xsl:stylesheet>

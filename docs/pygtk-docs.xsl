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
  <xsl:param name="refentry.separator" select="0"/>
  <xsl:param name="variablelist.as.table" select="1"/>

  <xsl:param name="gtkdoc.bookname" select="'pygtk-2.0'"/>

  <xsl:template match="article|book">
    <xsl:apply-imports/>

    <xsl:call-template name="generate.devhelp"/>
  </xsl:template>

  <xsl:template name="user.head.content">
    <style type="text/css">
      <xsl:text>
        .synopsis, .classsynopsis, .programlisting {
            background: #e0e0e0;
            border: solid 1pt #999999;
            padding: 4px;
        }
        .variablelist {
            background: #ffe9ca;
            border: solid 1pt #fdb558;
            padding: 4px;
        }
        .navigation {
            background: #f0d9d9;
            border: solid 1pt #ce8787;
            margin-top: 4pt;
            margin-bottom: 4pt;
        }
        .navigation a {
          color: #770000;
        }
        .navigation a:visited {
          color: #550000;
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

  <!-- nice looking heading -->
  <xsl:template name="header.navigation">
    <xsl:param name="prev" select="/foo"/>
    <xsl:param name="next" select="/foo"/>
    <xsl:variable name="home" select="/*[1]"/>
    <xsl:variable name="up" select="parent::*"/>

    <xsl:if test="$suppress.navigation = '0' and $home != .">
      <table class="navigation" width="100%"
             summary="Navigation header" cellpadding="2" cellspacing="0">
        <tr valign="center">
          <xsl:if test="count($prev) > 0">
            <td>
              <a accesskey="p">
                <xsl:attribute name="href">
                  <xsl:call-template name="href.target">
                    <xsl:with-param name="object" select="$prev"/>
                  </xsl:call-template>
                </xsl:attribute>
                <img src="left.png" width="24" height="24" border="0">
                  <xsl:attribute name="alt">
                    <xsl:call-template name="gentext">
                      <xsl:with-param name="key">nav-prev</xsl:with-param>
                    </xsl:call-template>
                  </xsl:attribute>
                </img>
              </a>
            </td>
          </xsl:if>
          <xsl:if test="count($up) > 0 and $up != $home">
            <td>
              <a accesskey="u">
                <xsl:attribute name="href">
                  <xsl:call-template name="href.target">
                    <xsl:with-param name="object" select="$up"/>
                  </xsl:call-template>
                </xsl:attribute>
                <img src="up.png" width="24" height="24" border="0">
                  <xsl:attribute name="alt">
                    <xsl:call-template name="gentext">
                      <xsl:with-param name="key">nav-up</xsl:with-param>
                    </xsl:call-template>
                  </xsl:attribute>
                </img>
              </a>
            </td>
          </xsl:if>
          <xsl:if test="$home != .">
            <td>
              <a accesskey="h">
                <xsl:attribute name="href">
                  <xsl:call-template name="href.target">
                    <xsl:with-param name="object" select="$home"/>
                  </xsl:call-template>
                </xsl:attribute>
                <img src="home.png" width="24" height="24" border="0">
                  <xsl:attribute name="alt">
                    <xsl:call-template name="gentext">
                      <xsl:with-param name="key">nav-home</xsl:with-param>
                    </xsl:call-template>
                  </xsl:attribute>
                </img>
              </a>
            </td>
          </xsl:if>
          <th width="100%" align="center">
            <xsl:apply-templates select="$home"
                                 mode="object.title.markup"/>
          </th>
          <xsl:if test="count($next) > 0">
            <td>
              <a accesskey="n">
                <xsl:attribute name="href">
                  <xsl:call-template name="href.target">
                    <xsl:with-param name="object" select="$next"/>
                  </xsl:call-template>
                </xsl:attribute>
                <img src="right.png" width="24" height="24" border="0">
                  <xsl:attribute name="alt">
                    <xsl:call-template name="gentext">
                      <xsl:with-param name="key">nav-next</xsl:with-param>
                    </xsl:call-template>
                  </xsl:attribute>
                </img>
              </a>
            </td>
          </xsl:if>
        </tr>
      </table>
    </xsl:if>
  </xsl:template>

  <xsl:template name="footer.navigation">
    <xsl:param name="prev" select="/foo"/>
    <xsl:param name="next" select="/foo"/>

    <xsl:if test="$suppress.navigation = '0'">
      <table class="navigation" width="100%"
             summary="Navigation footer" cellpadding="2" cellspacing="0">
        <tr valign="center">
          <td align="left">
            <xsl:if test="count($prev) > 0">
              <a accesskey="p">
                <xsl:attribute name="href">
                  <xsl:call-template name="href.target">
                    <xsl:with-param name="object" select="$prev"/>
                  </xsl:call-template>
                </xsl:attribute>
                <b>
                  <xsl:text>&lt;&lt;&#160;</xsl:text>
                  <xsl:apply-templates select="$prev"
                                       mode="object.title.markup"/>
                </b>
              </a>
            </xsl:if>
          </td>
          <td align="right">
            <xsl:if test="count($next) > 0">
              <a accesskey="p">
                <xsl:attribute name="href">
                  <xsl:call-template name="href.target">
                    <xsl:with-param name="object" select="$next"/>
                  </xsl:call-template>
                </xsl:attribute>
                <b>
                  <xsl:apply-templates select="$next"
                                       mode="object.title.markup"/>
                  <xsl:text>&#160;&gt;&gt;</xsl:text>
                </b>
              </a>
            </xsl:if>
          </td>
        </tr>
      </table>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>

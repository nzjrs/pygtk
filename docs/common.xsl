<?xml version='1.0'?> <!--*- mode: xml -*-->
<!DOCTYPE xsl:stylesheet [
]>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:param name="section.autolabel" select="1"/>
<xsl:param name="section.label.includes.component.label" select="1"/>
<xsl:param name="use.id.as.filename" select="1"/>

<xsl:template match="parameter">
	<xsl:choose>
		<xsl:when test="@role = 'keyword'">
			<xsl:call-template name="inline.boldmonoseq"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:call-template name="inline.italicmonoseq"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>


</xsl:stylesheet>

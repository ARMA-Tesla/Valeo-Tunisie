<!-- Sequence steps list XML to BKTree string (single step) conversion style sheet -->
<!-- DO NOT MODIFY IT -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text"/>
	<xsl:template match="*" />
	<xsl:strip-space elements="*"/>

  	<xsl:template match="/configuration">
		<xsl:apply-templates />
	</xsl:template>
	
	<xsl:template match="modules">
		<xsl:apply-templates />
	</xsl:template>
	
	<xsl:template match="module">
		<xsl:apply-templates />
	</xsl:template>

	<xsl:template match="*">
		<xsl:value-of select="name()"/>
		<xsl:text>="</xsl:text>
		<xsl:value-of select="text()"/>
		<xsl:text>"</xsl:text>
		<xsl:for-each select="@*">
			<xsl:text> @</xsl:text>
			<xsl:value-of select="name()"/>
			<xsl:text>="</xsl:text>
			<xsl:value-of select="."/>
			<xsl:text>"</xsl:text>
			<xsl:if test="not(position()=last())">,</xsl:if>
		</xsl:for-each>
		<xsl:text>; </xsl:text>
		<xsl:apply-templates />
	</xsl:template>
</xsl:stylesheet>
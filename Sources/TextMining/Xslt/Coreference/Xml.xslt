<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="html"/>

  <xsl:param name="main-font-size">10pt</xsl:param>

  <xsl:template match="/">
    <!--Courier, Tahoma-->

    <xsl:text disable-output-escaping="yes">&lt;div style="font-family: Tahoma; margin-bottom: 2em; font-size: </xsl:text>
    <xsl:value-of select="$main-font-size" />
    <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
      <xsl:apply-templates />
    <xsl:text disable-output-escaping="yes">&lt;/div&gt;</xsl:text>

  </xsl:template>

  <xsl:template match="*">
    <div style="margin-left: 1em; color: maroon;">
      &lt;<xsl:value-of select="local-name()"/><xsl:apply-templates select="@*"/>/&gt;
    </div>
  </xsl:template>

  <xsl:template match="*[node()]">
    <div style="margin-left: 1em;">
      <span style="color: maroon;">
        &lt;<xsl:value-of select="local-name()"/><xsl:apply-templates select="@*"/>&gt;</span>
      <xsl:apply-templates select="node()"/>
      <span style="color: maroon;">&lt;/<xsl:value-of select="local-name()"/>&gt;</span>
    </div>
  </xsl:template>

  <xsl:template match="text()">
    <xsl:choose>
      <xsl:when test="normalize-space() != '' or parent::U">
        <span style="color: silver;">
          &lt;![CDATA[<span style="color: gray;"><xsl:value-of select="." /></span>]]&gt;
        </span>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="." />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="@*">
    <span style="color: blue;">
      <xsl:text> </xsl:text>
      <xsl:value-of select="local-name()"/>="<span style="color: black;"><xsl:value-of select="." /></span>"</span>
  </xsl:template>
  
  <xsl:template match="@*[ (local-name() = 'COREF-MASTER') or
                           (local-name() = 'COREF-SLAVE') ]">
    <xsl:text> </xsl:text>
    <xsl:text disable-output-escaping="yes">&lt;span style="border: 1px dotted #000000; font-weight: bold;</xsl:text>
    <xsl:call-template name="get-color-by-coref-value">
      <xsl:with-param name="coref-value" select="." />
    </xsl:call-template>
    <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
    <xsl:value-of select="local-name()"/><xsl:text>="</xsl:text><xsl:value-of select="." /><xsl:text>"</xsl:text>
    <xsl:text disable-output-escaping="yes">&lt;/span&gt;</xsl:text>

  </xsl:template>

  <xsl:template name="get-color-by-coref-value">
    <xsl:param name="coref-value" />

    <xsl:choose>
      <xsl:when test="starts-with( $coref-value, 'PersonNameGroup-')">
        <xsl:text> color: white; background-color: blue; </xsl:text>
      </xsl:when>
      <xsl:when test="starts-with( $coref-value, 'PersonInfoGroup-')">
        <xsl:text> color: white; background-color: skyblue; </xsl:text>
      </xsl:when>
      <xsl:when test="starts-with( $coref-value, 'PersonNoInfoGroup-')">
        <xsl:text> color: white; background-color: violet; </xsl:text>
      </xsl:when>
      <xsl:when test="starts-with( $coref-value, 'OrgGroup-')">
        <xsl:text> color: white; background-color: green; </xsl:text>
      </xsl:when>
      <xsl:when test="starts-with( $coref-value, 'PlaceGroup-')">
        <xsl:text> color: white; background-color: red; </xsl:text>
      </xsl:when>
      <xsl:when test="starts-with( $coref-value, 'ProductGroup-')">
        <xsl:text> color: white; background-color: brown; </xsl:text>
      </xsl:when>
      <xsl:when test="starts-with( $coref-value, 'EventGroup-')">
        <xsl:text> color: white; background-color: gray; </xsl:text>
      </xsl:when>
      <xsl:when test="starts-with( $coref-value, 'OrgAbbrGroup-')">
        <xsl:text>background-color: yellow; </xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>background-color: #16ECE9; </xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
</xsl:stylesheet>
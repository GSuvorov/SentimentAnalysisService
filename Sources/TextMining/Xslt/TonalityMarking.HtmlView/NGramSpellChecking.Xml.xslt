<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="html"/>
  
  <xsl:template match="/">
    <!--Courier, Tahoma-->
    <div style="font-family: Tahoma; font-size: 10pt; margin-bottom: 2em;">
      <xsl:apply-templates />
    </div>
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

  <xsl:template match="text()" mode="without-cdata">
    <xsl:choose>
      <xsl:when test="normalize-space() != '' or parent::U">
        <span style="color: gray;"><xsl:value-of select="." /></span>
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

  <!--
  <xsl:template match="@*[ (local-name() = 'ow') ]">
    <xsl:text> </xsl:text>
    <span style="color: white; background-color: #ff9900; border: 1px dotted #000000; font-weight: bold;" title="original word">
      <xsl:value-of select="local-name()"/>="<span style="color: #990000;"><xsl:value-of select="." /></span>"</span>
  </xsl:template>
  -->
  <xsl:template match="@*[ (local-name() = 'owc') ]">
    <xsl:text> </xsl:text>
    <span style="color: white; background-color: #ff9900; border: 1px dotted #000000; font-weight: bold;" title="original word cleaned">
      <xsl:value-of select="local-name()"/>="<span style="color: #990000;"><xsl:value-of select="." /></span>"</span>
  </xsl:template>
  <xsl:template match="@*[ (local-name() = 'sw') ]">
    <xsl:text> </xsl:text>
    <span style="color: white; background-color: #ff9900; border: 1px dotted #000000; font-weight: bold;" title="suggest word">
      <xsl:value-of select="local-name()"/>="<span style="color: #990000;"><xsl:value-of select="." /></span>"</span>
  </xsl:template>
  <xsl:template match="*[ node() and (local-name() = 'e') ]">
    <div style="margin-left: 1em;">      
        <xsl:choose>
          <xsl:when test="@*">
            <span style="color: maroon;">
            &lt;<xsl:value-of select="local-name()"/><xsl:apply-templates select="@*"/>&gt;</span>
            <xsl:apply-templates select="node()" />
            <span style="color: maroon;">
            &lt;/<xsl:value-of select="local-name()"/>&gt;</span>
          </xsl:when>
          <xsl:otherwise>
            <span style="color: maroon;">
            &lt;<span style="color: #990000; background-color: #ff9900; border: 1px dotted #000000; font-weight: bold;"><xsl:value-of select="local-name()"/></span><xsl:apply-templates select="@*"/>&gt;</span>
            <xsl:apply-templates select="node()" />
            <span style="color: maroon;">
            &lt;/<span style="color: #990000; background-color: #ff9900; border: 1px dotted #000000; font-weight: bold;"><xsl:value-of select="local-name()"/></span>&gt;</span>
          </xsl:otherwise>
        </xsl:choose>
    </div>
  </xsl:template>

  <!--replace-quote-func-->
  <!--
    <xsl:call-template name="string-replace-all">
      <xsl:with-param name="text"    select="." />
      <xsl:with-param name="replace" select="'&quot;'" />
      <xsl:with-param name="by"      select="'&amp;quot;'" />
    </xsl:call-template>
  -->
  <!--
  <xsl:template name="string-replace-all">
    <xsl:param name="text" />
    <xsl:param name="replace" />
    <xsl:param name="by" />
    <xsl:choose>
      <xsl:when test="contains($text, $replace)">
        <xsl:value-of select="substring-before($text,$replace)" />
        <xsl:value-of select="$by" />
        <xsl:call-template name="string-replace-all">
          <xsl:with-param name="text"
          select="substring-after($text,$replace)" />
          <xsl:with-param name="replace" select="$replace" />
          <xsl:with-param name="by" select="$by" />
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$text" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  -->
  
</xsl:stylesheet>
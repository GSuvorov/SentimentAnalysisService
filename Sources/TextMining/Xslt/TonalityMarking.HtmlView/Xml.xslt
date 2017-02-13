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

  <xsl:template match="text()">
    <xsl:choose>
      <xsl:when test="normalize-space() != '' or parent::U">
        <!--<pre style="color: silver;">-->
        <span style="color: silver;">
          &lt;![CDATA[<span style="color: gray;"><xsl:value-of select="." /></span>]]&gt;
        </span>
        <!--</pre>-->
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="." />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <!--
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
  -->
  <!--<xsl:template match="cdata()">
    <pre>
      &lt;![CDATA[<xsl:value-of select="." />]]&gt;
    </pre>
  </xsl:template>-->

  <xsl:template match="@*">
    <span style="color: blue;">
      <xsl:text> </xsl:text>
      <xsl:value-of select="local-name()"/>="<span style="color: black;"><xsl:value-of select="." /></span>"</span>
  </xsl:template>
  
  <xsl:template match="@*[ (local-name() = 'SNT' and not(contains( ., 'Neut'))) or 
                           (local-name() = 'FRT' and . != 0) or
                           (local-name() = 'TYPE' and contains( ., 'Collocation')) ]">
    <xsl:text> </xsl:text>
    <span style="color: blue; border: 1px dotted #000000; font-weight: bold;">      
      <xsl:value-of select="local-name()"/>="<span style="color: black;"><xsl:value-of select="." /></span>"</span>
  </xsl:template>
  
  <xsl:template match="@*[ (local-name() = 'SNTFINAL' and not(contains( ., 'Neut'))) or 
                           (local-name() = 'FRTFINAL' and . != 0) ]">
    <xsl:text> </xsl:text>
    <xsl:choose>
      <xsl:when test="contains( ., 'Pos') or (local-name() = 'FRTFINAL' and contains( parent::*/@SNTFINAL, 'Pos'))">
        <span style="color: white; background-color: green; border: 1px dotted #000000; font-weight: bold;">
          <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"</span>
      </xsl:when>
      <xsl:when test="contains( ., 'Neg')or (local-name() = 'FRTFINAL' and contains( parent::*/@SNTFINAL, 'Neg'))">
        <span style="color: white; background-color: red; border: 1px dotted #000000; font-weight: bold;">
          <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"</span>
      </xsl:when>
      <xsl:otherwise>
        <span style="color: white; background-color: blue; border: 1px dotted #000000; font-weight: bold;">
          <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"</span>
      </xsl:otherwise>
    </xsl:choose>

  </xsl:template>

  <xsl:template match="@*[ (local-name() = 'INQUIRYINITIAL') ]">
    <xsl:text> </xsl:text>
    <span style="color: white; background-color: blue; border: 1px dotted #000000; font-weight: bold;">
      <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"</span>
  </xsl:template>

  <!--
  <xsl:template match="@*[ starts-with(local-name(), 'ow-') ]">
    <xsl:text> </xsl:text>
    <span style="color: white; background-color: #ff9900; border: 1px dotted #000000; font-weight: bold;" title="original word">
      <xsl:value-of select="local-name()"/>="<span style="color: #990000;"><xsl:value-of select="." /></span>"</span>
  </xsl:template>
  -->
  <xsl:template match="@*[ starts-with(local-name(), 'owc-') ]">
    <xsl:text> </xsl:text>
    <span style="color: white; background-color: #ff9900; border: 1px dotted #000000; font-weight: bold;" title="original word cleaned">
      <xsl:value-of select="local-name()"/>="<span style="color: #990000;"><xsl:value-of select="." /></span>"</span>
  </xsl:template>
  <xsl:template match="@*[ starts-with(local-name(), 'sw-') ]">
    <xsl:text> </xsl:text>
    <span style="color: white; background-color: #ff9900; border: 1px dotted #000000; font-weight: bold;" title="suggest word">
      <xsl:value-of select="local-name()"/>="<span style="color: #990000;"><xsl:value-of select="." /></span>"</span>
  </xsl:template>
  <!--
  <xsl:template match="*[ node() and (local-name() = 'e') ]">
    <div style="margin-left: 1em;">      
        <xsl:choose>
          <xsl:when test="@*">
            <span style="color: maroon;">
            &lt;<xsl:value-of select="local-name()"/><xsl:apply-templates select="@*"/>&gt;</span>
            <xsl:apply-templates select="node()" mode="without-cdata"/>
            <span style="color: maroon;">
            &lt;/<xsl:value-of select="local-name()"/>&gt;</span>
          </xsl:when>
          <xsl:otherwise>
            <span style="color: maroon;">
            &lt;<span style="color: #990000; background-color: #ff9900; border: 1px dotted #000000; font-weight: bold;"><xsl:value-of select="local-name()"/></span><xsl:apply-templates select="@*"/>&gt;</span>
            <xsl:apply-templates select="node()" mode="without-cdata"/>
            <span style="color: maroon;">
            &lt;/<span style="color: #990000; background-color: #ff9900; border: 1px dotted #000000; font-weight: bold;"><xsl:value-of select="local-name()"/></span>&gt;</span>
          </xsl:otherwise>
        </xsl:choose>
    </div>
  </xsl:template>
  -->
</xsl:stylesheet>
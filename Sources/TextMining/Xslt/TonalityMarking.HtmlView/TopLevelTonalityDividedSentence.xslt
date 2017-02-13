<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" 
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:msxsl="urn:schemas-microsoft-com:xslt" 
                exclude-result-prefixes="msxsl">
  <xsl:output method="html" indent="yes" />

  <xsl:variable name="PPosColor" >#C2D69B</xsl:variable>
  <xsl:variable name="PosColor"  >#33CC33</xsl:variable>
  <xsl:variable name="PNegColor" >#E69594</xsl:variable>
  <xsl:variable name="NegColor"  >#E02222</xsl:variable>
  <xsl:variable name="AmplfColor">#1578C3</xsl:variable>
  <xsl:variable name="BaseFontSize">12</xsl:variable>

  <xsl:template match="SENT">
    <hr style='width: 200px;' />
    <xsl:value-of select="position()" /><xsl:text>]. </xsl:text>
    <xsl:apply-templates select="node()"/>
  </xsl:template>
  
  <xsl:template match="node()[ @SNT ]">
    <xsl:element name="span">
      <xsl:attribute name="title">
        <xsl:call-template name="outputSNTnFRT" />
      </xsl:attribute>
      
      <xsl:choose>
        <xsl:when test="@TYPE = 'Inquiry'">
          <xsl:attribute name="style">
            <xsl:text>border: 1px dotted #000000; font-weight: bold;</xsl:text>
            <xsl:call-template name="SNT2foreColor" />
            <xsl:call-template name="FRT2fontSize" />
          </xsl:attribute>
        </xsl:when>
        <xsl:when test="contains( @SNT, 'Neut')" />
        <xsl:otherwise>
          <xsl:attribute name="style">
            <xsl:call-template name="SNT2foreColor" />
            <xsl:call-template name="FRT2fontSize" />
          </xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>

      <xsl:call-template name="output-text">
        <xsl:with-param name="txt" select="normalize-space(.)" />
      </xsl:call-template>
      
    </xsl:element>
  </xsl:template>

  <xsl:template name="FRT2fontSize">
    <xsl:if test="@FRT > 1">
      <xsl:text>font-size: </xsl:text>
      <xsl:choose>
        <xsl:when test="@FRT = 2">
          <xsl:value-of select="$BaseFontSize + 2" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$BaseFontSize + 4" />
        </xsl:otherwise>
      </xsl:choose>
      <xsl:text>pt;</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template name="outputSNTnFRT">
    <xsl:choose>
      <xsl:when test="@SNT='nNeut'">neutral</xsl:when>
      <xsl:when test="@SNT='nANeg'">action negative</xsl:when>
      <xsl:when test="@SNT='nAPos'">action positive</xsl:when>
      <xsl:when test="@SNT='nNeg'">negative</xsl:when>
      <xsl:when test="@SNT='nPos'">positive</xsl:when>
      <xsl:when test="@SNT='nPNeg'">potential negative</xsl:when>
      <xsl:when test="@SNT='nPPos'">potential positive</xsl:when>
      <xsl:when test="@SNT='vNeut'">neutral</xsl:when>
      <xsl:when test="@SNT='vFlxNeg'">reflexive negative</xsl:when>
      <xsl:when test="@SNT='vFlxPos'">reflexive positive</xsl:when>
      <xsl:when test="@SNT='vLink'">related verbs</xsl:when>
      <xsl:when test="@SNT='vNeg'">negative</xsl:when>
      <xsl:when test="@SNT='vPos'">positive</xsl:when>
      <xsl:when test="@SNT='vNegP'">negative pure</xsl:when>
      <xsl:when test="@SNT='vPosP'">positive pure</xsl:when>
      <xsl:when test="@SNT='vOppPos'">opposition positive</xsl:when>
      <xsl:when test="@SNT='vOppNeg'">opposition negative</xsl:when>
      <xsl:when test="@SNT='adjNeut'">neutral</xsl:when>
      <xsl:when test="@SNT='adjNeg'">negative</xsl:when>
      <xsl:when test="@SNT='adjPos'">positive</xsl:when>
      <xsl:when test="@SNT='adjAmplf'">amplificator</xsl:when>
      <xsl:when test="@SNT='advNeut'">neutral</xsl:when>
      <xsl:when test="@SNT='advNeg'">negative</xsl:when>
      <xsl:when test="@SNT='advPos'">positive</xsl:when>
      <xsl:when test="@SNT='advAmplf'">amplificator</xsl:when>
      <xsl:when test="@SNT='collNeut'">neutral</xsl:when>
      <xsl:when test="@SNT='collNeg'">negative</xsl:when>
      <xsl:when test="@SNT='collPos'">positive</xsl:when>
      <xsl:when test="@SNT='collAmplf'">amplificator</xsl:when>
      <xsl:when test="@SNT='collvNeg'">negative</xsl:when>
      <xsl:when test="@SNT='collvPos'">positive</xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="@SNT" />
      </xsl:otherwise>
    </xsl:choose>

    <xsl:if test="@FRT != 0">
      <xsl:text> (</xsl:text>
      <xsl:value-of select="@FRT" />
      <xsl:text>)</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template name="SNT2foreColor">
    <xsl:choose>
      <xsl:when test="contains( @SNT, 'PPos')" >color: <xsl:value-of select="$PPosColor"  />;</xsl:when>
      <xsl:when test="contains( @SNT, 'Pos')"  >color: <xsl:value-of select="$PosColor"   />;</xsl:when>
      <xsl:when test="contains( @SNT, 'PNeg')" >color: <xsl:value-of select="$PNegColor"  />;</xsl:when>
      <xsl:when test="contains( @SNT, 'Neg')"  >color: <xsl:value-of select="$NegColor"   />;</xsl:when>
      <xsl:when test="contains( @SNT, 'Amplf')">color: <xsl:value-of select="$AmplfColor" />;</xsl:when>     
      <xsl:otherwise />
    </xsl:choose>
  </xsl:template>

  <xsl:template name="output-text">
    <xsl:param name="txt" />

    <xsl:if test="$txt != ''">
      <xsl:if test="not(starts-with($txt, ',')) and 
                    not(starts-with($txt, ':')) and 
                    not(starts-with($txt, '.')) and 
                    not(starts-with($txt, '-')) and 
                    not(starts-with($txt, ';')) and 
                    not(starts-with($txt, '[')) and 
                    not(starts-with($txt, ']')) and 
                    not(starts-with($txt, '(')) and 
                    not(starts-with($txt, ')'))">
        <xsl:text> </xsl:text>
      </xsl:if>
      <xsl:value-of select="$txt" />
    </xsl:if>
  </xsl:template>

  <xsl:template match="node()">
    <xsl:choose>
      <xsl:when test="((./child::node())[ normalize-space() != '' ])[1] = ((./child::*)[1])[ normalize-space() != '' ]">
        <xsl:apply-templates select="node()"/>

        <xsl:call-template name="output-text">
          <xsl:with-param name="txt" select="normalize-space(./text())" />
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="./child::*">
        <xsl:call-template name="output-text">
          <xsl:with-param name="txt" select="normalize-space(./text())" />
        </xsl:call-template>

        <xsl:apply-templates select="./child::*"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="output-text">
          <xsl:with-param name="txt" select="normalize-space(.)" />
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <xsl:template match="/">
    <xsl:element name="div">
      <xsl:attribute name="style">
        <xsl:text>font-size: </xsl:text><xsl:value-of select="$BaseFontSize" /><xsl:text>pt;</xsl:text>
      </xsl:attribute>

      <xsl:apply-templates />
      
    </xsl:element>
  </xsl:template>  
  
</xsl:stylesheet>

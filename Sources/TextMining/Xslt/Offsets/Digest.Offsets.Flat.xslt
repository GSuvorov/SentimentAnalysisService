<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="html" indent="yes" />

  <xsl:variable name="PosColor">#33CC33</xsl:variable>  
  <xsl:variable name="NegColor">#E02222</xsl:variable>
  <xsl:variable name="BaseFontSize">10</xsl:variable>
  <xsl:variable name="NeutralTextForeColor">black</xsl:variable>
  
  <xsl:template match="/">
    <xsl:element name="div">
      <xsl:attribute name="style">
        <xsl:text>color: </xsl:text><xsl:value-of select="$NeutralTextForeColor" /><xsl:text>;</xsl:text>
        <xsl:text>font-size: </xsl:text><xsl:value-of select="$BaseFontSize" /><xsl:text>pt;</xsl:text>
      </xsl:attribute>

      <xsl:apply-templates />

    </xsl:element>
  </xsl:template>
  
  <xsl:template match="*[node()]">
    <xsl:apply-templates select="node()" />
  </xsl:template>
  
  <!--Разбивка на предложения
  <xsl:template match="SENT[node()]">
    <hr style='width: 200px;' /><xsl:value-of select="position()" /><xsl:text>]. </xsl:text>
    <xsl:apply-templates select="node()"/>
  </xsl:template>
  -->   

  <xsl:template match="text()">
    <xsl:call-template name="output-text">
      <xsl:with-param name="txt" select="normalize-space()" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="*" priority="-9" />

  <!--объект-->
  <xsl:template match="object-begin">
    <xsl:text> </xsl:text>
    <xsl:text disable-output-escaping="yes">
      &lt;span style='border: 1px dotted #000000; font-weight: bold;' title='объект'&gt;
    </xsl:text>    
    <xsl:apply-templates select="node()" />      
  </xsl:template>
  <xsl:template match="object-end">  
    <xsl:text disable-output-escaping="yes">&lt;/span&gt;</xsl:text>
  </xsl:template>  

  <!--субъект-->
  <xsl:template match="subject-begin">
    <xsl:text> </xsl:text>
    <xsl:text disable-output-escaping="yes">
      &lt;span style='border: 1px dotted #000000; font-weight: bold; font-style: italic; color: White; background-color: Gray' title='субъект'&gt;
    </xsl:text>    
    <xsl:apply-templates select="node()" />      
  </xsl:template>
  <xsl:template match="subject-end">    
    <xsl:text disable-output-escaping="yes">&lt;/span&gt;</xsl:text>
  </xsl:template>
  
  <!--тональность-->
  <xsl:template match="positive-tonality-begin">
    <xsl:text> </xsl:text>
    <xsl:text disable-output-escaping="yes">&lt;span style='</xsl:text>
    <xsl:call-template name="snt-to-fore-color"><xsl:with-param name="snt" select="string('pos')"/></xsl:call-template>
    <xsl:call-template name="frt-to-font-size" />    
    <xsl:text disable-output-escaping="yes">'&gt;</xsl:text>
    <xsl:apply-templates select="node()" />
  </xsl:template>
  <xsl:template match="negative-tonality-begin">
    <xsl:text> </xsl:text>
    <xsl:text disable-output-escaping="yes">&lt;span style='</xsl:text>
    <xsl:call-template name="snt-to-fore-color"><xsl:with-param name="snt" select="string('neg')"/></xsl:call-template>
    <xsl:call-template name="frt-to-font-size" />
    <xsl:text disable-output-escaping="yes">'&gt;</xsl:text>
    <xsl:apply-templates select="node()" />
  </xsl:template>
  <xsl:template match="neutral-tonality-begin">
    <xsl:text> </xsl:text>
    <xsl:text disable-output-escaping="yes">&lt;span style='</xsl:text>
    <xsl:text>color: </xsl:text><xsl:value-of select="$NeutralTextForeColor" /><xsl:text>;</xsl:text>
    <xsl:text>font-size: </xsl:text><xsl:value-of select="$BaseFontSize" /><xsl:text>pt;</xsl:text>
    <xsl:text disable-output-escaping="yes">'&gt;</xsl:text>
    <xsl:apply-templates select="node()" />
  </xsl:template>
  <xsl:template match="positive-tonality-end | negative-tonality-end | neutral-tonality-end">
    <xsl:text disable-output-escaping="yes">&lt;/span&gt;</xsl:text>
  </xsl:template>
  
  <!--прямая и косвенная речь-->
  <xsl:template match="direct-speech-begin | indirect-speech-begin">
    <xsl:text disable-output-escaping="yes">&lt;i&gt;</xsl:text>
    <xsl:apply-templates select="node()" />
  </xsl:template>
  <xsl:template match="direct-speech-end | indirect-speech-end">
    <xsl:apply-templates select="node()" />
    <xsl:text disable-output-escaping="yes">&lt;/i&gt;</xsl:text>
  </xsl:template>  

  <!--выделение анафории-->
  <xsl:template match="anaphoria-begin">
    <xsl:text disable-output-escaping="yes">&lt;span title='</xsl:text>
    <xsl:value-of select="@anaphoria"/>
    <xsl:text disable-output-escaping="yes">'&gt;</xsl:text>
    <xsl:apply-templates select="node()" />
  </xsl:template>
  <xsl:template match="anaphoria-end">    
    <xsl:text disable-output-escaping="yes">&lt;/span&gt;</xsl:text>
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

  <xsl:template name="frt-to-font-size">
    <xsl:if test="@frt > 1">
      <xsl:text>font-size: </xsl:text>
      <xsl:choose>
        <xsl:when test="@frt = 2">
          <xsl:value-of select="$BaseFontSize + 2" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$BaseFontSize + 4" />
        </xsl:otherwise>
      </xsl:choose>
      <xsl:text>pt;</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template name="output-snt-and-frt">
    <xsl:choose>
      <xsl:when test="@snt='nNeut'">neutral</xsl:when>
      <xsl:when test="@snt='nANeg'">action negative</xsl:when>
      <xsl:when test="@snt='nAPos'">action positive</xsl:when>
      <xsl:when test="@snt='nNeg'">negative</xsl:when>
      <xsl:when test="@snt='nPos'">positive</xsl:when>
      <xsl:when test="@snt='nPNeg'">potential negative</xsl:when>
      <xsl:when test="@snt='nPPos'">potential positive</xsl:when>
      <xsl:when test="@snt='vNeut'">neutral</xsl:when>
      <xsl:when test="@snt='vFlxNeg'">reflexive negative</xsl:when>
      <xsl:when test="@snt='vFlxPos'">reflexive positive</xsl:when>
      <xsl:when test="@snt='vLink'">related verbs</xsl:when>
      <xsl:when test="@snt='vNeg'">negative</xsl:when>
      <xsl:when test="@snt='vPos'">positive</xsl:when>
      <xsl:when test="@snt='vNegP'">negative pure</xsl:when>
      <xsl:when test="@snt='vPosP'">positive pure</xsl:when>
      <xsl:when test="@snt='vOppPos'">opposition positive</xsl:when>
      <xsl:when test="@snt='vOppNeg'">opposition negative</xsl:when>
      <xsl:when test="@snt='adjNeut'">neutral</xsl:when>
      <xsl:when test="@snt='adjNeg'">negative</xsl:when>
      <xsl:when test="@snt='adjPos'">positive</xsl:when>
      <xsl:when test="@snt='adjAmplf'">amplificator</xsl:when>
      <xsl:when test="@snt='advNeut'">neutral</xsl:when>
      <xsl:when test="@snt='advNeg'">negative</xsl:when>
      <xsl:when test="@snt='advPos'">positive</xsl:when>
      <xsl:when test="@snt='advAmplf'">amplificator</xsl:when>
      <xsl:when test="@snt='collNeut'">neutral</xsl:when>
      <xsl:when test="@snt='collNeg'">negative</xsl:when>
      <xsl:when test="@snt='collPos'">positive</xsl:when>
      <xsl:when test="@snt='collAmplf'">amplificator</xsl:when>
      <xsl:when test="@snt='collvNeg'">negative</xsl:when>
      <xsl:when test="@snt='collvPos'">positive</xsl:when>
      <xsl:otherwise><xsl:value-of select="@snt" /></xsl:otherwise>
    </xsl:choose>

    <xsl:if test="@frt != 0">
      <xsl:text> (</xsl:text><xsl:text>final fortress: </xsl:text><xsl:value-of select="@frt" /><xsl:text>)</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template name="snt-to-fore-color">
    <xsl:param name="snt" />
    <xsl:text>color: </xsl:text>
    <xsl:choose>
      <xsl:when test="$snt = 'pos'"><xsl:value-of select="$PosColor" /></xsl:when>      
      <xsl:when test="$snt = 'neg'"><xsl:value-of select="$NegColor" /></xsl:when>
      <xsl:otherwise><xsl:value-of select="$NeutralTextForeColor" /></xsl:otherwise>
    </xsl:choose>
    <xsl:text>;</xsl:text>
  </xsl:template>
  
</xsl:stylesheet>
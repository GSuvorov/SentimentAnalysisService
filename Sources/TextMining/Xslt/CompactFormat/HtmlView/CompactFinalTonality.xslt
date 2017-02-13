<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="html" indent="yes" />

  <xsl:variable name="PPosColor" >#C2D69B</xsl:variable>
  <xsl:variable name="PosColor"  >#33CC33</xsl:variable>
  <xsl:variable name="PNegColor" >#E69594</xsl:variable>
  <xsl:variable name="NegColor"  >#E02222</xsl:variable>
  <xsl:variable name="AmplfColor">#1578C3</xsl:variable>
  <xsl:variable name="BaseFontSize">12</xsl:variable>
  <xsl:variable name="NeutralTextForeColor">black</xsl:variable>
  
  <xsl:template match="/">
    <xsl:element name="div">
      <xsl:attribute name="style">
        <xsl:text>font-size: </xsl:text><xsl:value-of select="$BaseFontSize" /><xsl:text>pt;</xsl:text>
        <xsl:text>color: </xsl:text><xsl:value-of select="$NeutralTextForeColor" /><xsl:text>;</xsl:text>
        <xsl:text>padding: 2px;</xsl:text>
      </xsl:attribute>

      <xsl:apply-templates />

    </xsl:element>
  </xsl:template>
  
  <xsl:template match="*[node()]">
    <xsl:choose>
      <xsl:when test="@snt or local-name() = 'inquiry'">
        <xsl:if test="local-name() = 'inquiry'">
          <xsl:text> </xsl:text>
        </xsl:if>
        
        <xsl:element name="span">
          <xsl:attribute name="title">
            <xsl:call-template name="output-snt-and-frt" />
          </xsl:attribute>

          <xsl:choose>
            <xsl:when test="local-name() = 'inquiry'">
              <xsl:attribute name="style">
                <xsl:if test="@snt">
                  <xsl:call-template name="snt-to-fore-color" />
                  <xsl:call-template name="frt-to-font-size" />
                </xsl:if>
                <xsl:text>border: 1px dotted #000000; font-weight: bold;</xsl:text>
              </xsl:attribute>
              
              <xsl:value-of select="normalize-space()"/>
            </xsl:when>
            <xsl:otherwise>
              <!--<xsl:if test="@snt">-->
              <xsl:attribute name="style">
                <xsl:call-template name="snt-to-fore-color" />
                <xsl:call-template name="frt-to-font-size" />
              </xsl:attribute>
              <!--</xsl:if>-->
              
              <xsl:apply-templates select="node()"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:element>
      </xsl:when>

      <xsl:when test="local-name() = 'subject'">
        <xsl:call-template name="subject-template" />
      </xsl:when>

      <xsl:when test="local-name() = 'direct-speech'">
        <xsl:call-template name="direct-speech-template" />
      </xsl:when>
      <xsl:when test="local-name() = 'indirect-speech'">
        <xsl:call-template name="indirect-speech-template" />
      </xsl:when>

      <xsl:when test="local-name() = 'anaphoria'">
        <xsl:call-template name="anaphoria-template" />
      </xsl:when>

      <xsl:otherwise>
        <xsl:apply-templates select="node()" />
      </xsl:otherwise>
    </xsl:choose>
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

  <!--субъект-->
  <xsl:template name="subject-template">
    <xsl:text> </xsl:text>
    <xsl:element name="span">
      <xsl:attribute name="style">
        <xsl:text>border: 1px dotted #000000; font-weight: bold; font-style: italic; color: White; background-color: Gray</xsl:text>
      </xsl:attribute>
      <xsl:attribute name="title">субъект</xsl:attribute>
      <!--<xsl:value-of select="normalize-space()"/>-->

      <xsl:apply-templates select="node()" />

    </xsl:element>
  </xsl:template>

  <!--прямая и косвенная речь-->
  <xsl:template name="direct-speech-template">
    <i>
      <xsl:apply-templates select="node()" />
    </i>
  </xsl:template>
  <xsl:template name="indirect-speech-template">
    <i>
      <xsl:apply-templates select="node()" />
    </i>
  </xsl:template>

  <!--выделение анафории-->
  <xsl:template name="anaphoria-template">
    <xsl:element name="span">
      <xsl:attribute name="title">
        <xsl:value-of select="@value"/>
      </xsl:attribute>
      <xsl:apply-templates select="node()" />
    </xsl:element>
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
    <xsl:text>color: </xsl:text>
    <xsl:choose>
      <xsl:when test="@snt = 'nPos'"><xsl:value-of select="$PosColor" /></xsl:when>      
      <xsl:when test="@snt = 'nNeg'"><xsl:value-of select="$NegColor" /></xsl:when>
      <!--
      <xsl:when test="@snt = 'PPos'" ><xsl:value-of select="$PPosColor"  /></xsl:when>
      <xsl:when test="@snt = 'PNeg'" ><xsl:value-of select="$PNegColor"  /></xsl:when>
      <xsl:when test="@snt = 'Amplf'"><xsl:value-of select="$AmplfColor" /></xsl:when>-->
      <xsl:otherwise><xsl:value-of select="$NeutralTextForeColor" /></xsl:otherwise>
    </xsl:choose>
    <xsl:text>;</xsl:text>
  </xsl:template>
  
</xsl:stylesheet>
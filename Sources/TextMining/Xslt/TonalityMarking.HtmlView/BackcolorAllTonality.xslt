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
  
  <xsl:template match="node()[ @SNT or @SNTFINAL ]">
    <xsl:element name="span">
      <xsl:attribute name="title">
        <xsl:call-template name="SNTnFRT2text" />
      </xsl:attribute>

      <xsl:attribute name="style">
        <xsl:variable name="padding">
          <xsl:choose>
            <xsl:when test="count(descendant::*[@SNT]/*[@SNT]) = 0">
              <xsl:value-of select="count((child::*[@SNT])[ 1 ]) * 2"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="count(descendant::*[@SNT]/*[@SNT]) * 2"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        <xsl:if test="$padding != 0">
          <xsl:text>height: 1px;</xsl:text>
          <xsl:text>padding: </xsl:text><xsl:value-of select="$padding" /><xsl:text>px;</xsl:text>
        </xsl:if>

        <xsl:choose>
          <xsl:when test="@TYPE = 'Inquiry'">
            <xsl:text>border: 1px dotted #000000; font-weight: bold;</xsl:text>
            <xsl:call-template name="SNT2backColor" />
            <xsl:call-template name="FRT2fontSize" />
          </xsl:when>
          <xsl:when test="contains( @SNT, 'Neut')" />
          <xsl:otherwise>
            <xsl:text>border: solid 1px silver;</xsl:text>
            <xsl:call-template name="SNT2backColor" />
            <xsl:call-template name="FRT2fontSize" />
          </xsl:otherwise>          
        </xsl:choose>
      </xsl:attribute>
      
      <!--LAST-PREVIOUS
      <xsl:call-template name="output-text">
        <xsl:with-param name="txt" select="normalize-space( ./text() )" />
      </xsl:call-template>

      <xsl:apply-templates select="node()"/>
      -->
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
  <!--<xsl:template name="FRT2fontSize">
    <xsl:if test="@FRT != 0">
      <xsl:text>font-size: </xsl:text>
      <xsl:choose>
        <xsl:when test="@FRT > 3">
          <xsl:value-of select="$BaseFontSize + 3 * 2" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$BaseFontSize + @FRT * 2" />
        </xsl:otherwise>
      </xsl:choose>
      <xsl:text>pt;</xsl:text>
    </xsl:if>
  </xsl:template>-->
  
  <xsl:template name="SNTnFRT2text">
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

  <xsl:template name="SNT2backColor">
    <xsl:choose>
      <xsl:when test="contains( @SNT, 'PPos')" >background-color: <xsl:value-of select="$PPosColor"  />;</xsl:when>
      <xsl:when test="contains( @SNT, 'Pos')"  >background-color: <xsl:value-of select="$PosColor"   />;</xsl:when>
      <xsl:when test="contains( @SNT, 'PNeg')" >background-color: <xsl:value-of select="$PNegColor"  />;</xsl:when>
      <xsl:when test="contains( @SNT, 'Neg')"  >background-color: <xsl:value-of select="$NegColor"   />;</xsl:when>
      <xsl:when test="contains( @SNT, 'Amplf')">background-color: <xsl:value-of select="$AmplfColor" />;</xsl:when>
      
      <!--<xsl:when test="@SNT='nANeg'">background-color: Red;</xsl:when>
      <xsl:when test="@SNT='nAPos'">background-color: #33CC33;</xsl:when>
      <xsl:when test="@SNT='nNeg'">background-color: Red;</xsl:when>
      <xsl:when test="@SNT='nPos'">background-color: #33CC33;</xsl:when>
      <xsl:when test="@SNT='nPNeg'">background-color: Pink;</xsl:when>
      <xsl:when test="@SNT='nPPos'">background-color: #00FF99;</xsl:when>
      <xsl:when test="@SNT='vFlxNeg'">background-color: Red;</xsl:when>
      <xsl:when test="@SNT='vFlxPos'">background-color: #33CC33;</xsl:when>
      <xsl:when test="@SNT='vLink'">related verbs</xsl:when>
      <xsl:when test="@SNT='vNeg'">background-color: Red;</xsl:when>
      <xsl:when test="@SNT='vPos'">background-color: #33CC33;</xsl:when>
      <xsl:when test="@SNT='vNegP'">background-color: Red;</xsl:when>
      <xsl:when test="@SNT='vPosP'">background-color: #33CC33;</xsl:when>
      <xsl:when test="@SNT='vOppPos'">background-color: #33CC33;</xsl:when>
      <xsl:when test="@SNT='vOppNeg'">background-color: Red;</xsl:when>
      <xsl:when test="@SNT='adjNeg'">background-color: Red;</xsl:when>
      <xsl:when test="@SNT='adjPos'">background-color: #33CC33;</xsl:when>
      <xsl:when test="@SNT='adjAmplf'">amplificator</xsl:when>
      <xsl:when test="@SNT='advNeg'">background-color: Red;</xsl:when>
      <xsl:when test="@SNT='advPos'">background-color: #33CC33;</xsl:when>
      <xsl:when test="@SNT='advAmplf'">amplificator</xsl:when>
      <xsl:when test="@SNT='collNeg'">background-color: Red;</xsl:when>
      <xsl:when test="@SNT='collPos'">background-color: #33CC33;</xsl:when>
      <xsl:when test="@SNT='collAmplf'">amplificator</xsl:when>
      <xsl:when test="@SNT='collvNeg'">background-color: Red;</xsl:when>
      <xsl:when test="@SNT='collvPos'">background-color: #33CC33;</xsl:when>-->
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

  <!--LAST-PREVIOUS
  <xsl:template match="node()">
    <xsl:call-template name="output-text">
      <xsl:with-param name="txt" select="normalize-space( text() )" />
    </xsl:call-template>
    
    <xsl:apply-templates select="node()"/>
  </xsl:template>
  -->
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

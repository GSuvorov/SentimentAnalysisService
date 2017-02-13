<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="html" indent="yes" />

  <xsl:variable name="PPosColor" >#C2D69B</xsl:variable>
  <xsl:variable name="PosColor"  >#33CC33</xsl:variable>
  <xsl:variable name="PNegColor" >#E69594</xsl:variable>
  <xsl:variable name="NegColor"  >#E02222</xsl:variable>
  <xsl:variable name="AmplfColor">#1578C3</xsl:variable>
  <xsl:variable name="BaseFontSize">10</xsl:variable>
  <xsl:variable name="NeutralTextForeColor">black</xsl:variable>
  <xsl:variable name="SubjectTitle">субъект</xsl:variable>
  <xsl:variable name="ObjectTitle">объект</xsl:variable>
  
  <xsl:template match="/">
    <xsl:element name="div">
      <xsl:attribute name="style">
        <xsl:text>color: </xsl:text><xsl:value-of select="$NeutralTextForeColor" /><xsl:text>;</xsl:text>
        <xsl:text>font-size: </xsl:text><xsl:value-of select="$BaseFontSize" /><xsl:text>pt;</xsl:text>
      </xsl:attribute>

      <xsl:apply-templates />

    </xsl:element>
  </xsl:template>

  <xsl:template match="*[node()]" name="main-node-processing">
    <xsl:choose>
      <xsl:when test="@SNT or @SNTFINAL">

        <!--чтобы убрать пробелы-отступы для рамочки INQUIRYINITIAL--> 
        <xsl:if test="@INQUIRYINITIAL = 'TRUE'">
          <xsl:text> </xsl:text>
        </xsl:if>
        
        <xsl:element name="span">          
          <!--убрали tooltip's
          <xsl:attribute name="title">
            <xsl:call-template name="outputSNTnFRT" />
          </xsl:attribute>
          -->

          <xsl:choose>
            <xsl:when test="@TYPE = 'Inquiry'">              
              <xsl:attribute name="style">
                <xsl:if test="@SNTFINAL">
                  <xsl:call-template name="SNTFINAL2foreColor" />
                  <xsl:call-template name="FRTFINAL2fontSize" />
                </xsl:if>
                <xsl:if test="@INQUIRYINITIAL = 'TRUE'">
                  <xsl:text>border: 1px dotted #000000; font-weight: bold;</xsl:text>
                </xsl:if>
              </xsl:attribute>
              
              <!--чтобы убрать пробелы-отступы для рамочки INQUIRYINITIAL-->
              <xsl:choose>
                <xsl:when test="@INQUIRYINITIAL = 'TRUE'">
                  <xsl:call-template name="anaphoria">
                    <xsl:with-param name="additional-value" select="$ObjectTitle" />
                  </xsl:call-template>
                  <xsl:value-of select="normalize-space()"/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:apply-templates select="node()"/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:when>
            <xsl:when test="not(@SNTFINAL) and contains(@SNT, 'Neut')">
              <xsl:apply-templates select="node()" />
            </xsl:when>
            <xsl:otherwise>
              <xsl:if test="@SNTFINAL">
                <xsl:attribute name="style">
                  <xsl:call-template name="SNTFINAL2foreColor" />
                  <xsl:call-template name="FRTFINAL2fontSize" />
                </xsl:attribute>
              </xsl:if>
              
              <!--чтобы убрать пробелы-отступы для рамочки INQUIRYINITIAL-->
              <xsl:apply-templates select="node()"/>
            </xsl:otherwise>
          </xsl:choose>

          <!--чтобы убрать пробелы-отступы для рамочки INQUIRYINITIAL-->
          <!--<xsl:apply-templates select="node()"/>-->
        </xsl:element>

      </xsl:when>
      
      <xsl:otherwise>
        <xsl:apply-templates select="node()" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!--выделение анафории-->
  <xsl:template match="node()[ @LINK and @FULLNAME and (local-name()='ENTITY' or local-name()='CET') ]">
    <xsl:element name="span">
      <xsl:attribute name="title">
        <xsl:value-of select="@FULLNAME"/>
      </xsl:attribute>
      <xsl:call-template name="main-node-processing" />
    </xsl:element>
  </xsl:template>
  <xsl:template name="anaphoria">
    <xsl:param name="additional-value" />
    <xsl:attribute name="title">
      <xsl:if test="@LINK and @FULLNAME and (local-name()='ENTITY' or local-name()='CET')">
        <xsl:value-of select="@FULLNAME"/>
        <xsl:text>, </xsl:text>
      </xsl:if>
      <xsl:value-of select="$additional-value" />
    </xsl:attribute>
  </xsl:template>

  <xsl:template match="node()[ @ISSUBJECT and not(@INQUIRYINITIAL) ]">
    <xsl:choose>      
      <xsl:when test="not(ancestor::SENT//*[ @ISSUBJECT and @INQUIRYINITIAL ])">
        <xsl:text> </xsl:text>
        <xsl:element name="span">
          <xsl:attribute name="style">
            <xsl:text>border: 1px dotted #000000; font-weight: bold; font-style: italic; color: White; background-color: Gray</xsl:text>
          </xsl:attribute>
          <xsl:call-template name="anaphoria">
            <xsl:with-param name="additional-value" select="$SubjectTitle" />
          </xsl:call-template>

          <xsl:apply-templates select="node()" />

        </xsl:element>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="node()" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template> 
  
  <xsl:template match="node()[local-name()='SUB_SENT' and ((not(@SNT) and not(@SNTFINAL)) or (@SNT='nNeut' and @SNTFINAL='nNeut'))]">
    <xsl:element name="span">
      <xsl:attribute name="style">
        <xsl:text>color: </xsl:text><xsl:value-of select="$NeutralTextForeColor" /><xsl:text>;</xsl:text>
        <xsl:text>font-size: </xsl:text><xsl:value-of select="$BaseFontSize" /><xsl:text>pt;</xsl:text>
      </xsl:attribute>

      <xsl:apply-templates select="node()" />

    </xsl:element>        
  </xsl:template>

  <xsl:template match="node()[ @INDIRECTSPEECH-BEGIN or @DIRECTSPEECH-BEGIN ]">
    <xsl:text disable-output-escaping="yes">&lt;i&gt;</xsl:text>
    <xsl:call-template name="main-node-processing" />
    <xsl:if test="@INDIRECTSPEECH-END or @DIRECTSPEECH-END">
      <xsl:text disable-output-escaping="yes">&lt;/i&gt;</xsl:text>
    </xsl:if>
  </xsl:template>
  <xsl:template match="node()[ @INDIRECTSPEECH-END or @DIRECTSPEECH-END ]">
    <xsl:if test="@INDIRECTSPEECH-BEGIN or @DIRECTSPEECH-BEGIN">
      <xsl:text disable-output-escaping="yes">&lt;i&gt;</xsl:text>
    </xsl:if>
    <xsl:call-template name="main-node-processing" />
    <xsl:text disable-output-escaping="yes">&lt;/i&gt;</xsl:text>
  </xsl:template>

  <!--Разбивка на предложения
  <xsl:template match="SENT[node() and not(ancestor::SUB_SENT[ @TYPE = 'DirectSpeech' ])]">
    <xsl:apply-templates select="node()"/>
    <br />
    <br />
  </xsl:template>
  -->

  <xsl:template match="text()">
    <xsl:call-template name="output-text">
      <xsl:with-param name="txt" select="normalize-space()" />
    </xsl:call-template>
  </xsl:template>
  
  <xsl:template match="*" priority="-9" />

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

  <xsl:template name="FRTFINAL2fontSize">
    <xsl:if test="@FRTFINAL > 1">
      <xsl:text>font-size: </xsl:text>
      <xsl:choose>
        <xsl:when test="@FRTFINAL = 2">
          <xsl:value-of select="$BaseFontSize + 2" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$BaseFontSize + 4" />
        </xsl:otherwise>
      </xsl:choose>
      <xsl:text>pt;</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template name="SNTFINAL2foreColor">
    <xsl:choose>
      <xsl:when test="contains( @SNTFINAL, 'PPos')" >color: <xsl:value-of select="$PPosColor"  />;</xsl:when>
      <xsl:when test="contains( @SNTFINAL, 'Pos')"  >color: <xsl:value-of select="$PosColor"   />;</xsl:when>
      <xsl:when test="contains( @SNTFINAL, 'PNeg')" >color: <xsl:value-of select="$PNegColor"  />;</xsl:when>
      <xsl:when test="contains( @SNTFINAL, 'Neg')"  >color: <xsl:value-of select="$NegColor"   />;</xsl:when>
      <xsl:when test="contains( @SNTFINAL, 'Amplf')">color: <xsl:value-of select="$AmplfColor" />;</xsl:when>      
      <xsl:otherwise />
    </xsl:choose>
  </xsl:template>
  
</xsl:stylesheet>
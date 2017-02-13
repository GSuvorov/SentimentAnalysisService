<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:msxsl="urn:schemas-microsoft-com:xslt"
                exclude-result-prefixes="msxsl"
                >
  <xsl:output method="xml" indent="yes"/>
  <xsl:strip-space elements="text"/>
  
  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>
  
  <!--CONTINUE CHAIN-->
  <xsl:template match="tonality" mode="chain">
    <xsl:variable select="@snt" name="snt"/>
    <xsl:variable select="@frt" name="frt"/>
    <xsl:variable select="(following-sibling::*)[1]" name="next-sibling"/>
    
    <xsl:choose>

      <xsl:when test="((following-sibling::node())[ normalize-space() != '' ])[1] = $next-sibling[ normalize-space() != '' ]
                       and
                      $next-sibling[ local-name() = 'tonality' ] and 
                      $next-sibling[ (not(@snt) and not($snt)) or (@snt = $snt) ] and 
                      $next-sibling[ (not(@frt) and not($frt)) or (@frt = $frt) ]">

        <!--<xsl:message terminate="no">chain bingo. need to make union node!!!!!!!!!</xsl:message>-->
        <xsl:copy-of select="./node()"/>
        <xsl:apply-templates select="$next-sibling" mode="chain" />

      </xsl:when>

      <xsl:otherwise>
        <xsl:copy-of select="./node()"/>
      </xsl:otherwise>

    </xsl:choose>
  </xsl:template>
  
  <!--BEGIN OF CHAIN-->
  <xsl:template match="tonality">
    <xsl:variable select="@snt" name="snt" />
    <xsl:variable select="@frt" name="frt" />
    <xsl:variable select="(preceding-sibling::*)[last()]" name="previous-sibling" />
    <xsl:variable select="(following-sibling::*)[1]"      name="next-sibling" />
    
    <xsl:choose>

      <xsl:when test="((preceding-sibling::node())[ normalize-space() != '' ])[last()] = $previous-sibling[ normalize-space() != '' ] 
                       and 
                      $previous-sibling[ local-name() = 'tonality' ] and 
                      $previous-sibling[ (not(@snt) and not($snt)) or (@snt = $snt) ] and 
                      $previous-sibling[ (not(@frt) and not($frt)) or (@frt = $frt) ]">
        <!--<xsl:message terminate="no">[goto on penis]</xsl:message>-->
      </xsl:when>
      
      <xsl:when test="((following-sibling::node())[ normalize-space() != '' ])[1] = $next-sibling[ normalize-space() != '' ]
                       and
                      $next-sibling[ local-name() = 'tonality' ] and 
                      $next-sibling[ (not(@snt) and not($snt)) or (@snt = $snt) ] and 
                      $next-sibling[ (not(@frt) and not($frt)) or (@frt = $frt) ]">

        <!--<xsl:message terminate="no">bingo. need to make union node!!!!!!!!!</xsl:message>-->
        <xsl:copy>
          <xsl:apply-templates select="@* | ./node()"/>
          <xsl:apply-templates select="$next-sibling" mode="chain" />
        </xsl:copy>

      </xsl:when>

      <xsl:otherwise>
        <xsl:copy>
          <xsl:apply-templates select="@* | node()"/>
        </xsl:copy>
      </xsl:otherwise>

    </xsl:choose>

  </xsl:template> 

</xsl:stylesheet>

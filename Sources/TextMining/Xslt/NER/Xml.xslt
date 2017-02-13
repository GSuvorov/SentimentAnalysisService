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
        &lt;<xsl:value-of select="local-name()"/><xsl:apply-templates select="@*"/>&gt;
      </span>
      <xsl:apply-templates select="node()"/>
      <span style="color: maroon;">
        &lt;/<xsl:value-of select="local-name()"/>&gt;
      </span>
    </div>
  </xsl:template>

  <xsl:template match="text()">
    <xsl:choose>
      <xsl:when test="normalize-space() != '' or parent::U">
        <!--<pre style="color: silver;">-->
        <span style="color: silver;">
          &lt;![CDATA[<span style="color: gray;">
            <xsl:value-of select="." />
          </span>]]&gt;
        </span>
        <!--</pre>-->
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="." />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:param name="organizationHighlight" select="true()"/>
  <xsl:param name="dateHighlight" select="true()"/>
  <xsl:param name="numericHighlight" select="true()"/>
  <xsl:param name="personHighlight" select="true()"/>
  <xsl:param name="placeHighlight" select="true()"/>
  <xsl:param name="unknownProperHighlight" select="true()"/>
  <xsl:param name="addressHighlight" select="true()"/>
  <xsl:param name="emailHighlight" select="true()"/>
  <xsl:param name="bankAccountHighlight" select="true()"/>
  <xsl:param name="urlHighlight" select="true()"/>
  <xsl:param name="phoneHighlight" select="true()"/>
  <xsl:param name="innHighlight" select="true()"/>
  <xsl:param name="moneyHighlight" select="true()"/>
  <xsl:param name="sloganHighlight" select="true()"/>
  <xsl:param name="indexHighlight" select="true()"/>
  <xsl:param name="homogeneousHighlight" select="true()"/>
  <xsl:param name="weightHighlight" select="true()"/>
  <xsl:param name="distanceHighlight" select="true()"/>
  <xsl:param name="pretextHighlight" select="true()"/>
  <xsl:param name="productHighlight" select="true()"/>
  <xsl:param name="eventHighlight" select="true()"/>

  <!--Цвета здесь http://html-color-codes.info/-->
  <xsl:param name="organizationColor">blue</xsl:param>
  <xsl:param name="dateColor">lime</xsl:param>
  <xsl:param name="numericColor">#DF01D7</xsl:param>
  <xsl:param name="personColor">#F2F5A9</xsl:param>
  <xsl:param name="placeColor">#FACC2E</xsl:param>
  <xsl:param name="unknownProperColor">#BE81F7</xsl:param>
  <xsl:param name="addressColor">#0040FF</xsl:param>
  <xsl:param name="emailColor">#2ECCFA</xsl:param>
  <xsl:param name="bankAccountColor">#80FF00</xsl:param>
  <xsl:param name="urlColor">#9F70D5</xsl:param>
  <xsl:param name="phoneColor">#70D5C1</xsl:param>
  <xsl:param name="innColor">#89D570</xsl:param>
  <xsl:param name="moneyColor">#CBD570</xsl:param>
  <xsl:param name="sloganColor">#FFEF00</xsl:param>
  <xsl:param name="indexColor">#00FFF7</xsl:param>
  <xsl:param name="homogeneousColor">#AA8802</xsl:param>
  <xsl:param name="weightColor">#04C3A0</xsl:param>
  <xsl:param name="distanceColor">#9D4900</xsl:param>
  <xsl:param name="pretextColor">#9DDD00</xsl:param>
  <xsl:param name="productColor">red</xsl:param>
  <xsl:param name="eventColor">olive</xsl:param>

  <!--<param name="elems" type="Array">
    		array The Array to find items in.
  </param>-->
  <!--///	<param name="proxy" type="Object"> //Boolean
    ///		The object to which the scope of the function should be set.
    ///
  </param>-->

  <xsl:template match="@*">
    <span style="color: blue;">
      <xsl:text> </xsl:text>
      <xsl:value-of select="local-name()"/>="<span style="color: black;">
        <xsl:value-of select="." />
      </span>"
    </span>
  </xsl:template>

  <!-- Вырезать
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
  -->

  <xsl:template name="Main">
    <xsl:if test="local-name()='ENTITY'">
      <xsl:call-template name="MatchAttributes" />
    </xsl:if>
  </xsl:template>

  <xsl:template name="MatchCet" match="@*[ (local-name() = 'TYPE')]">

    <xsl:choose>

      <!--Числительные и числа-->
      <xsl:when test=". = 'Numeric' and $numericHighlight">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$numericColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--Предлог-->
      <xsl:when test="$pretextHighlight and . = 'Pretext'">
        <xsl:text> </xsl:text>
        <span style="color: white; background-color: {$pretextColor}; border: 1px dotted #000000; font-weight: bold;">
          <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
        </span>
      </xsl:when>

      <xsl:otherwise>
        <xsl:call-template name="MatchDefaultType" />
      </xsl:otherwise>
      
    </xsl:choose>

  </xsl:template>

  <xsl:template name="MatchEntities" match="@*[ (local-name() = 'TYPE')]">
    <xsl:choose>

      <!--Аббревиатура, Юридическое лицо, Тип организации-->
      <xsl:when test="$organizationHighlight and (. = 'OrgAbbr' or . = 'OrgName' or . = 'OrgNoInfo')">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$organizationColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--Продукты-->
      <xsl:when test="$productHighlight and . = 'Product'">
        <xsl:text> </xsl:text>
        <span style="color: white; background-color: {$productColor}; border: 1px dotted #000000; font-weight: bold;">
          <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
        </span>
      </xsl:when>

      <!--События-->
      <xsl:when test="$eventHighlight and . = 'Event'">
        <xsl:text> </xsl:text>
        <span style="color: white; background-color: {$eventColor}; border: 1px dotted #000000; font-weight: bold;">
          <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
        </span>
      </xsl:when>

      <!--Диапазон времени-->
      <xsl:when test="$dateHighlight and . = 'Date'">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$dateColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--Персона с фамилией, Профессия или должность-->
      <xsl:when test="$personHighlight and (. = 'PersonName' or . = 'PersonInfo' or . = 'PersonNoInfo')">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$personColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--Географическое название, Тип географического объекта, Географический объект, не прошедший проверки по словарю-->
      <xsl:when test="$placeHighlight and (. = 'PlaceName' or . = 'PlaceNoInfo'  or . = 'PseudoGeo')">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$placeColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--Неидентифицированные объекты-->
      <xsl:when test="$unknownProperHighlight and (. = 'UnknownProper')">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$unknownProperColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--Адрес, Улица, Дом, Корпус, Строение, Квартира-->
      <xsl:when test="$addressHighlight and (. = 'Address' or . = 'Street' or . = 'House' or . = 'Corps' or . = 'Building' or . = 'Flat')">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$addressColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--Электронная почта-->
      <xsl:when test="$emailHighlight and . = 'Email'">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$emailColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--Банковский счет-->
      <xsl:when test="$bankAccountHighlight and . = 'BankAccount'">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$bankAccountColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--URL-->
      <xsl:when test="$urlHighlight and . = 'URL'">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$urlColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--Номер телефона-->
      <xsl:when test="$phoneHighlight and . = 'Phone'">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$phoneColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--ИНН-->
      <xsl:when test="$innHighlight and . = 'INN'">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$innColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--Денежные данные-->
      <xsl:when test="$moneyHighlight and . = 'Money'">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$moneyColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--Лозунги-->
      <xsl:when test="$sloganHighlight and . = 'Slogan'">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$sloganColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--Индексы-->
      <xsl:when test="$indexHighlight and . = 'Index'">
        <xsl:text> </xsl:text>
          <span style="color: white; background-color: {$indexColor}; border: 1px dotted #000000; font-weight: bold;">
            <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
          </span>
      </xsl:when>

      <!--Однородные-->
      <xsl:when test="$homogeneousHighlight and . = 'Homogeneous'">
        <xsl:text> </xsl:text>
        <span style="color: white; background-color: {$homogeneousColor}; border: 1px dotted #000000; font-weight: bold;">
          <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
        </span>
      </xsl:when>

      <!--Вес-->
      <xsl:when test="$weightHighlight and . = 'Weight'">
        <xsl:text> </xsl:text>
        <span style="color: white; background-color: {$weightColor}; border: 1px dotted #000000; font-weight: bold;">
          <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
        </span>
      </xsl:when>

      <!--Расстояние-->
      <xsl:when test="$distanceHighlight and . = 'Distance'">
        <xsl:text> </xsl:text>
        <span style="color: white; background-color: {$distanceColor}; border: 1px dotted #000000; font-weight: bold;">
          <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
        </span>
      </xsl:when>

      <!--Не выделяемые сущности-->
      <xsl:when test=". = 'Adjective' or . = 'Pronoun' or . = 'Adverb' or . = 'Homogeneous' or . = 'Unknown' or . = 'NounVerb'">
        <xsl:call-template name="MatchDefaultType" />
      </xsl:when>

      <xsl:otherwise>
        <xsl:call-template name="MatchDefaultType" />
      </xsl:otherwise>

    </xsl:choose>

  </xsl:template>

  <xsl:template name="MatchDefaultType" match="@*[ (local-name() = 'TYPE')]">
    <span style="color: blue;">
      <xsl:text> </xsl:text>
      <xsl:value-of select="local-name()"/>="<span style="color: black;">
        <xsl:value-of select="." />
      </span>"
    </span>
  </xsl:template>


  <xsl:template name="MatchAttributes" match="@*[ (local-name() = 'TYPE')]">

    <xsl:choose>
      <xsl:when test="local-name(parent::*) = 'ENTITY'">
        <xsl:call-template name="MatchEntities"/>
      </xsl:when>

      <xsl:when test="local-name(parent::*)='CET'">
        <xsl:call-template name="MatchCet" />
      </xsl:when>

      <xsl:otherwise>
        <xsl:call-template name="MatchDefaultType" />
      </xsl:otherwise>
    </xsl:choose>




  </xsl:template>


  <!--<xsl:template match="@*[ (local-name() = 'TYPE') and . = 'ENTR' ]">
    <xsl:text> </xsl:text>
    <span style="color: white; background-color: fuchsia; border: 1px dotted #000000; font-weight: bold;">
      <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
    </span>
  </xsl:template>-->

  <!--<xsl:template match="@*[ (local-name() = 'TYPE') and . = 'PROD' ]">
    <xsl:text> </xsl:text>
    <span style="color: white; background-color: orange; border: 1px dotted #000000; font-weight: bold;">
      <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
    </span>
  </xsl:template>-->

  <!--<xsl:template match="@*[ (local-name() = 'TYPE') and . = 'Name' ]">
    <xsl:text> </xsl:text>
    <span style="color: white; background-color: red; border: 1px dotted #000000; font-weight: bold;">
      <xsl:value-of select="local-name()"/>="<xsl:value-of select="." />"
    </span>
  </xsl:template>-->

</xsl:stylesheet>

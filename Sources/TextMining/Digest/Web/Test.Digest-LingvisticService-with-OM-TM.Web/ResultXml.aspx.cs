using System;
using System.Web;

using Digest;

namespace Test.Digest.Web
{
    public partial class ResultXml : PageBase
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            XmlToResponse( this.CurrentDigestOutputResult );

            this.CurrentDigestOutputResult = null;
        }
    }
}

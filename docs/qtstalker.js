window.onload = function() {
 var oDiv = document.getElementById("version");
 oDiv.innerHTML = "Version 0.37-dev<br />(other <a href='faq.html#versions'>versions</a>)";
 var oDiv2 = document.getElementById("warnDoc");
 if (oDiv2 != null) {
  oDiv2.innerHTML = "<strong>Warning:</strong><br />This is documentation for the previous version, and is not yet updated.";
 }
}

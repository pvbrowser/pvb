#!/bin/bash
rlbb2html -back=Back -home=Content -forward=Forward -language=en -title=Primer "-banner=pvbrowser primer" -hmax=3
rlbb2html "-back=Zur&uuml;ck" -home=Inhalt -forward=Vor -language=de -title=Primer "-banner=pvbrowser primer" -hmax=3
rlbb2html -total -language=en
rlbb2html -total -language=de


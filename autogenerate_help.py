#! /usr/bin/env python

#from BeautifulSoup import BeautifulSoup as BSHTML
import urllib2
import re

url = 'http://northernwidget.com/alog/new-to-arduino-start-here/'
response = urllib2.urlopen(url)

lines_raw = response.readlines()
lines = []

for line in lines_raw:
  newline = re.sub(r'[^\x00-\x7F]+', ' ', line)
  newline = re.sub(r'`', "'", newline)
  newline = re.sub(r'&#8217;', "'", newline)
  newline = re.sub(r'&#8212;', "--", newline)
  newline = re.sub(r'&#8220;', '"', newline)
  newline = re.sub(r'&#8221;', '"', newline)
  if "ll start" in newline:
    print newline
  lines.append(newline)
  
#i=0
#for line in lines:
#  if '<title>' in line:
#    nHeaderEndLine = i
#    break

i=0
for line in lines:
  if '<div class="entry-content">' in line:
    nStartLine = i+1
  if "</div><!-- .entry-content -->" in line:
    nEndLine = i
  i += 1

f = open('doc/construct_html/new_users_guide.html', 'w+')
#f.writelines('/*! \mainpage ALog Guide\r\n')
#f.write('\r\n')
#for line in lines[:nHeaderEndLine]:
#  f.write(line)
for line in lines[nStartLine:nEndLine]:
  f.write(line)
#for line in lines[-2:]: # end body, HTML
#  f.write(line)
f.write('\n')
f.write(' */')
f.close()


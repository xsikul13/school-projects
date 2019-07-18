#! /usr/bin/env python3
import sys
import struct
import socket
import re
import requests
import urllib3
import bencodepy  # s laskavým svolením autorů bencodepy pod licencí uvedenou v souboru LICENCE-bencodepy
import hashlib
import urllib
# import modulu pro práci s argumenty příkazové řádky
import argparse
import xml.etree.ElementTree as ET


#parsovani argumentu prikazove radky, pomoci argparse
def argumenty():
  parser = argparse.ArgumentParser(description='Projekt isa 2015', epilog='by xsikul13')
  parser.add_argument('-r','--rss',
                   help='URL adresa RSS feedu\nnapr.: ./antipirat -r https://kat.cr/movies/?rss=1')
  parser.add_argument('-i','--input-announcement',
                   help='uložený RSS feed v souboru, napr. testing_movies_announce.xml ')
  parser.add_argument('-a','--tracker-annonce-url','--tracker-announce-url',
                   help='podvržené URL trackeru pro získaní peerlistu\nnapr.: ./antipirat -r https://kat.cr/movies/?rss=1 -a http://tracker.pl/announce  ')
  parser.add_argument('-t','--torrent-file',
                   help=' již stažený torrent, bez použití RSS\nnapr.: ./antipirat -t 2010.the.year.we.make.contact.1984.brrip.h264.torrent ')
  
  args = parser.parse_args()                 
  #print(args)  
  return args
 
 
#stazeni xml souboru ze zadane url (kat)  
def DownloadFile(url):
  try:
    r = requests.get(url)
  except:
    sys.stderr.write("spatne url")
    exit(9)
  rss_xml = r.content.decode("utf-8")
  f = open('movies_announce.xml', 'w')
  f.write(rss_xml) 
  f.close()
    
  return rss_xml


#pruchod xml souborem - vytovreni movies_announce_list.txt
def movie_list(rss_xml):
  try:
    root = ET.fromstring(rss_xml) 
  except:
    sys.stderr.write("spatne xml\n")
    exit(10)
  m = re.search("xmlns:torrent=\".*?\"", rss_xml)
  x = m.group(0)
  m = re.search("\".*?\"", x)
  xmlns = m.group(0)
  
  f = open("movies_announce_list.txt", "w")
  
  s1 = root[0][0].tag
  s1 += ": "
  s1 += root[0][0].text
  f.write(s1)
  f.write("\n")
  s1 = root[0][1].tag
  s1 += ": "
  s1 += root[0][1].text
  f.write(s1)
  f.write("\n")
  s1 = root[0][2].tag
  s1 += ": "
  s1 += root[0][2].text
  f.write(s1)
  f.write("\n")  
  f.write("\n")
 
  items = root[0].findall("item")
  #print(len(items))
  
  #print(items[1].tag)
  #print(items[1].text)
  
  name = "nic"
  link = "nic"
  for item in items:
    f.write("\n")
    #print(item[12].tag)
    #print(item[12].text)
    #link 1. torrentu, který budeme stahovat
    
    if link == "nic":
      
      link = item.find("enclosure").get("url")
      name = item.find(str("{"+xmlns[1:-1]+"}fileName")).text
      
      #link = "https://torcache.net/torrent/"
      #link += item[7].text
      #link += ".torrent?title=[kat.cr]"
      #link += item[12].text
      #name = item[12].text

                    
   # print(item[0].text)
     
    s1 = "title: "
    i = item.find("title")
    if (ET.iselement(i)):
      s1 += i.text      
    f.write(s1)
    f.write("\n")
            
    s1 = "category: "
    i = item.find("category")
    if (ET.iselement(i)):
      s1 += i.text      
    f.write(s1)
    f.write("\n")

    s1 = "author: "
    i = item.find("author")
    if (ET.iselement(i)):
      s1 += i.text      
    f.write(s1)
    f.write("\n")
    
    s1 = "link: "
    i = item.find("link")
    if (ET.iselement(i)):
      s1 += i.text      
    f.write(s1)
    f.write("\n")
    
    s1 = "pubDate: "
    i = item.find("pubDate")
    if (ET.iselement(i)):
      s1 += i.text      
    f.write(s1)
    f.write("\n")
    
    s1 = "torrent:infoHash: "
    i = item.find("{"+xmlns[1:-1]+"}infoHash")
    if (ET.iselement(i)):
      s1 += i.text      
    f.write(s1)
    f.write("\n")
         
    s1 = "torrent:fileName: "
    i = item.find("{"+xmlns[1:-1]+"}fileName")
    if (ET.iselement(i)):
      s1 += i.text
      if (name == "nic"):
        name = i.text      
    f.write(s1)
    f.write("\n") 
     
  
  f.close()
  return link[0:-8], name
  

  
#stazeni torrent souboru 
def download_torrent(url, name):
  
  http = urllib3.PoolManager()
  r = http.request('GET', url)
  if r.data[-7:] == b"</html>":
    sys.stderr.write("nepodarilo se stahnout torrent\n")
    exit()
    #return -1
    
  f = open(name, "wb")  
  f.write(r.data)
  return r.data


#prozkoumani torrent souboru
#nalezeni http trackeru
def check_torrent(t):
  trackers_url = list()
  #print("\n\n"+str(t)+"\n\n")
  try:
    tt = bencodepy.decode(t)
  except:
    sys.stderr.write("spatny bendcode torrentu\n")
    #print(t)
    return -1  
  #print(hashlib.sha1(t).hexdigest())
  info_hash = hashlib.sha1()
  info_hash = hashlib.sha1(bencodepy.encode(tt[b"info"]))
  
  i_h = info_hash.hexdigest()
  a = range(0,39,2)
  ih = ""
  for aa in a:
    ih += "%"+i_h[aa:aa+2].upper()
  
  if str(args.tracker_annonce_url) != "None": # obsluha parametru -a --tracker_annonce_url URL
    a = args.tracker_annonce_url.encode()
    #a += b"announce"
    b = list()
    b.append(a)
    make_request(b, ih, i_h)
  else:                                       # klasika bez podvrzeneho trackera
    #print(urllib.parse.quote(info_hash))
    for a in tt[b"announce-list"]:
      make_request(a, ih, i_h)

#vytvoreni GET pozadavku    
def make_request(a, ih, i_h):
    #print(a)
    #print(a[0].decode())
    #print(a)
    if a[0].decode()[0:4] == "http":    
      #trackers_url.append(a[0].decode())
      tr = a[0].decode()
      urld = tr[7+tr[7:].find("/"):]+"?"
      urld += "info_hash="+ih
      urld += "&uploaded=0&downloaded=0&compact=0&event=started"
      urld += "&peer_id=12345678987654321234&port=6881&left=4863&numwant=200"
      
      #print(urld)
      i = tr[7:].find(":")
      if i != -1:
        k = tr[7:].find("/")
        host = tr[7:7+i]
        #print(tr[8+i:7+k])
        port = int(tr[8+i:7+k])
      else:
        port = 80
        k = tr[7:].find("/")
        host = tr[7:7+k] 
      request = "GET "+urld+" HTTP/1.1\r\nHost: "+host+"\r\n\r\n"  
      
      
      get_peers(host, port, request, i_h)  
      return

#pripojeni se na na tracker
#parsovani jeho odpovedi
#ulozeni do souboru [i_h].peerslist
def get_peers(host, port, request, i_h):      
      s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
      
      
      print("port: "+str(port)+" host: "+ host)        
      s.settimeout(10)
      try:
        s.connect((host, port))
      except socket.error as msg:
        sys.stderr.write("nepodarilo se pripojit na: "+host+"\n")
        return -1
      print("připojeno")
      
            
      s.send(request.encode())
      
      #print(request)
      odpoved = b""
      while True:
        try: 
          a =  s.recv(1024)
        except:
          sys.stderr.write("vyprsel timeout\n")
          return -1
        if len(a) == 0:
          break
        odpoved += a
      #if (len(odpoved) == 0):
      #  print("chyba")  
      s.close()  
      #print(odpoved)
      if ( odpoved.find(b"200 OK\r\n\r\n") == -1):
        sys.stderr.write("spatna odpoved trackeru\n")
        #sys.stderr.write(str(odpoved))
        return
        
      bodpoved = odpoved[odpoved.find(b"\r\n\r\n")+4:]
      try:
        bb = bencodepy.decode(bodpoved)
      except:
        sys.stderr.write("spatny bendcode\n")
        return -1
      #print(bb[b"peers"])
      
      decoded = bodpoved # decode the bencoded announce
      binary_ip = bb[b"peers"]
      #print(binary_ip) # this will be a multiple of 6 (ie, 12 = 2 ip:port)
      l = len(bb[b"peers"])
      
      offset = 0
      ipp = set()
      while offset != l:

        ip1 = struct.unpack_from("!i", binary_ip, offset)[0] # ! = network order(big endian); i = int
        first_ip = socket.inet_ntoa(struct.pack("!i", ip1))
        offset +=4 # save where the first ip ends and the port begins
        port1 = struct.unpack_from("!H", binary_ip, offset)[0] # H = unsigned short
        offset += 2 
        #print(str(first_ip)+":"+str(port1))
        ipp.add(str(first_ip)+":"+str(port1))
        
      print("nalezeno ip adres: "+str(len(ipp)))  
      f = open(i_h+".peerlist","a")
      for item in ipp:
        f.write(item)
        f.write("\n")
      f.close()
      
      return 
      


if __name__ == '__main__':
  args = argumenty()
  #url = "https://kat.cr/movies/?rss=1"
  #rss_xml = DownloadFile(url)
  #link, name = movie_list(rss_xml)
  #torrent = download_torrent(link, name)
  #check_torrent(torrent)

  if str(args.rss) != "None":
    url = args.rss
    rss_xml = DownloadFile(url)
    link, name = movie_list(rss_xml)
    torrent = download_torrent(link, name)
    check_torrent(torrent)
   
  elif str(args.input_announcement) != "None":
    print("input: "+ args.input_announcement)
    f = open(args.input_announcement, "r")
    rss_xml = f.read()
    link, name = movie_list(rss_xml)
    torrent = download_torrent(link, name)
    check_torrent(torrent)  

 # if str(args.tracker_annonce_url) != "None": obsluha v ramci check_torrent()

  elif str(args.torrent_file) != "None":
    f = open(args.torrent_file, "rb")
    check_torrent(f.read())
  else:
    sys.stderr.write("spatne vstupni parametry\n")
    exit(13)


 

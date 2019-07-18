import tweepy
import json
import sys
import re
import requests
import os

 
# Consumer keys and access tokens, used for OAuth
consumer_key = 'tl1aYHyICBwl8fYdExLNVHR9t'
consumer_secret = 'BtvGWzimiDEsibP5eiHHmjDm7y3y6j3Go2eoW4z322zy4yiyBj'
access_token = '3241842591-j7S5Q1gKHydGCpGkqbCSjULxxpScnSDD3sqS886'
access_token_secret = 'HJ48lPNJxMG877zjNcDsuEqibEPjeKCk3jhSssbNO5SaF'

def get_all_tweets(screen_name, pocet_tweetu):
	#Twitter only allows access to a users most recent 3240 tweets with this method
	
	#authorize twitter, initialize tweepy
	auth = tweepy.OAuthHandler(consumer_key, consumer_secret)
	auth.set_access_token(access_token, access_token_secret)
	api = tweepy.API(auth)
	
	#initialize a list to hold all the tweepy Tweets
	alltweets = []	
	
	if not os.path.exists("tweets-rogerfederer"):
		os.makedirs("tweets-rogerfederer")
	a = "adsf"

	os.chdir('tweets-rogerfederer')
	a = "asf"
	
	lastID = 0
	if not os.path.exists("conf"):
	    f_id = open('conf', "w")
	    status_list = api.user_timeline(screen_name, count=pocet_tweetu,include_entities=True)
	else:
		f_id = open('conf')
		o = f_id.read()
		lastID = o
		status_list = api.user_timeline(screen_name, count=pocet_tweetu,since_id = lastID, include_entities=True)
	    
	f_id.close()
	  
	
	
    #make initial request for most recent tweets (200 is the maximum allowed count)
	#status_list = api.user_timeline(screen_name,include_entities=True)
	for status in status_list:
	    json_str = json.dumps(status._json)
	    json_data = json.loads(json_str)
	    ##print(json_data['text']+"\n")
	    urls = json_data['entities']
	    id = json_data['id'];
	    lastID = id if id > lastID else lastID
	    
	    ff = open('tweets', "a+")
	    ff.write(str(id)+':\t'+json_data['text'].replace('\n', '\\n')+"\n")
	    
	    print("id: \t"+str(id))
	    #if urls != '[]':
	    #     url = urls['url']
	    #print(urls)
	    #p_url = re.search('\'urls\': \[\{\'url\': \'((?:https?:\/\/)?(?:[\da-z\.-]+)\.(?:[a-z\.]{2,6})(?:[\/\w \.-]*)*\/?)')
	    #p_url = re.compile(r"(\'.*?\')")  '((?:https?:\/\/)?(?:[\da-z\.-]+)\.(?:[a-z\.]{2,6})(?:[\/\w \.-]*)*\/?)'
	    sstr = json.dumps(urls)
	    
	    #print(sstr)
	    try:
	        p = re.search('\"urls\": \[\{.*?\"url\": \"(?P<url>(https?:\/\/)?([\da-z\.-]+)\.([a-z\.]{2,6})([\/\w \.-]*)*\/?)', sstr)
	        #print(p)
	        #print("wtf");
	        print("stahuji:\t"+p.group('url'))
	        r = requests.get(p.group('url'))
	        f = open(str(id), "w")
	        f.write(r);
	        
	        
	        close(f)
	        print(len(r.content))
	        
	        #print("hejjjjjj\t"+p_url)
	    except AttributeError:
	        pass
	    except TypeError:
	        pass
	    
	        
	    
	        
	    #if urls['expanded_url'])
	    
	    #print(json_data['entities'])
	    #print(json_str)
	    print("------------------------------------")
	    
	##print(str(lastID))
	f_id = open('conf', "w")
	f_id.write('{}'.format(lastID))
	f_id.close()
	
	ff.close()

 
 
if __name__ == '__main__':
	#pass in the username of the account you want to download
	pocet_tweetu = 50
	get_all_tweets("rogerfederer", pocet_tweetu)

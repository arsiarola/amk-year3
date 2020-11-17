from http.server import BaseHTTPRequestHandler, HTTPServer
import json

hostName = "localhost"
hostPort = 9000

temp = 0
level = 0

class MyServer(BaseHTTPRequestHandler):

	def do_GET(self):
		self.send_response(200)
		self.send_header('Content-Type', 'application/json')
		self.send_header("Access-Control-Allow-Origin", "*")
		self.end_headers()
		if self.path == '/getData':
			print("getData called")
			self.wfile.write(json.dumps({'temperature': 5}).encode())


myServer = HTTPServer((hostName, hostPort), MyServer)

try:
	myServer.serve_forever()
except KeyboardInterrupt:
	pass

myServer.server_close()

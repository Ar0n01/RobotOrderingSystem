import bottle
from bottle import request, response
import json
import os
import threading

@bottle.route('/votes', method = 'POST')
def update_votes():
        print("Running")
        vote_data = request.forms.get("votes")

        if not vote_data:
                return bottle.HTTPResponse(status = 400, body = "missing >

        try:
                votes = json.loads(vote_data)
        except json.JSONDecodeError:
                return bottle.HTTPResponse(status = 400, body = "invalid >

        with open(os.path.expanduser("~/public_html/votes.json"), "w") as>
                json.dump(votes, f)

        print("Votes updated")
        return bottle.HTTPResponse(
                status=200,
                body=json.dumps({"status": "success"}),
                headers={'Content-Type': 'application/json'}
        )

if __name__ == '__main__':
        bottle.run(host='::', port=7213)
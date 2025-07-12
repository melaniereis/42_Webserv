#!/usr/bin/python3 python3
import os
import html
import sys

print("Content-Type: text/html\r\n\r\n")

# Helper to get env variables safely
def get_env(var, default='N/A'):
	return html.escape(os.environ.get(var, default))

method = get_env('REQUEST_METHOD')
query_string = get_env('QUERY_STRING', '')
remote_addr = get_env('REMOTE_ADDR')
user_agent = get_env('HTTP_USER_AGENT')
server_software = get_env('SERVER_SOFTWARE')
server_protocol = get_env('SERVER_PROTOCOL')

# Handle POST data for form demo
form_response = ""
if method == "POST":
	try:
		content_length = int(os.environ.get("CONTENT_LENGTH", 0))
		post_data = sys.stdin.read(content_length) if content_length > 0 else ""
		# Parse form data (very basic, not robust)
		fields = {}
		for pair in post_data.split("&"):
			if "=" in pair:
				k, v = pair.split("=", 1)
				fields[k] = html.unescape(v.replace("+", " "))
		name = html.escape(fields.get("name", ""))
		email = html.escape(fields.get("email", ""))
		message = html.escape(fields.get("message", ""))
		if name and email and message:
			form_response = f"""
			<div class="response">
				<strong>Thank you, {name}!</strong><br>
				<span>Your message has been received.</span><br>
				<em>Email:</em> {email}<br>
				<em>Message:</em> {message}
			</div>
			"""
	except Exception:
		form_response = '<div class="response" style="background:#e74c3c22;border:1px solid #e74c3c;">Error parsing form data.</div>'

print(f"""<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Python CGI Test</title>
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<style>
		:root {{
			--primary: #6C63FF;
			--primary-dark: #5651d9;
			--secondary: #FF6584;
			--accent: #00C9A7;
			--dark: #121826;
			--darker: #0A0E17;
			--light: #F0F5FF;
			--card-bg: rgba(25, 30, 48, 0.7);
			--transition: all 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275);
			--shadow: 0 10px 30px rgba(0, 0, 0, 0.3);
		}}
		* {{
			margin: 0;
			padding: 0;
			box-sizing: border-box;
			font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
		}}
		body {{
			background: radial-gradient(ellipse at top, var(--darker), transparent),
				radial-gradient(ellipse at bottom, #1a1f3a, transparent),
				linear-gradient(to bottom, #0a0e17, #121826);
			color: var(--light);
			min-height: 100vh;
			overflow-x: hidden;
			line-height: 1.6;
			background-attachment: fixed;
			display: flex;
			justify-content: center;
			align-items: center;
		}}
		body::before {{
			content: "";
			position: fixed;
			top: 0;
			left: 0;
			width: 100%;
			height: 100%;
			background:
				radial-gradient(circle at 15% 30%, rgba(108, 99, 255, 0.08) 0%, transparent 25%),
				radial-gradient(circle at 85% 70%, rgba(255, 101, 132, 0.08) 0%, transparent 25%);
			z-index: -1;
			pointer-events: none;
		}}
		.container {{
			max-width: 500px;
			margin: 7vh auto 0 auto;
			padding: 2rem;
		}}
		.feature-card {{
			background: var(--card-bg);
			border-radius: 20px;
			padding: 2.5rem;
			transition: var(--transition);
			border: 1px solid rgba(255, 255, 255, 0.08);
			backdrop-filter: blur(10px);
			position: relative;
			overflow: hidden;
			box-shadow: var(--shadow);
			text-align: center;
		}}
		.feature-card:hover {{
			transform: translateY(-10px) scale(1.02);
			box-shadow: 0 20px 40px rgba(0, 0, 0, 0.4);
			border-color: rgba(108, 99, 255, 0.3);
		}}
		.logo-icon {{
			width: 50px;
			height: 50px;
			background: linear-gradient(135deg, var(--primary), var(--accent));
			border-radius: 12px;
			display: flex;
			align-items: center;
			justify-content: center;
			font-size: 2.3rem;
			color: white;
			box-shadow: var(--shadow);
			margin: 0 auto 1.2rem auto;
		}}
		.logo-text {{
			font-size: 1.8rem;
			font-weight: 700;
			background: linear-gradient(to right, var(--light), var(--accent));
			-webkit-background-clip: text;
			-webkit-text-fill-color: transparent;
			margin-bottom: 1.2rem;
		}}
		.status-badge {{
			margin-bottom: 1.5rem;
			display: inline-block;
			background: var(--primary);
			color: #fff;
			padding: 0.6em 1.5em;
			border-radius: 2em;
			font-weight: 600;
			font-size: 1.05em;
			box-shadow: 0 2px 8px rgba(108,99,255,0.12);
		}}
		.info-block {{
			text-align: left;
			margin: 1.8rem 0 0 0;
			background: rgba(255,255,255,0.03);
			padding: 1.2em 1em;
			border-radius: 10px;
			font-size: 1.08em;
		}}
		.info-block p {{
			margin: 10px 0;
			color: #F0F5FF;
		}}
		.form input[type="text"],
		.form input[type="email"],
		.form textarea {{
			padding: 0.6rem 1rem;
			font-size: 1rem;
			border-radius: 8px;
			border: none;
			margin-bottom: 0.7rem;
			outline: none;
			background: #1e263b;
			color: white;
			width: 100%;
			box-sizing: border-box;
		}}
		.form textarea {{
			resize: vertical;
			min-height: 80px;
		}}
		.form input[type="submit"], .form button {{
			padding: 0.6rem 1.2rem;
			background: var(--primary);
			border: none;
			color: white;
			border-radius: 8px;
			cursor: pointer;
			transition: var(--transition);
			font-size: 1rem;
			font-weight: 600;
			width: 100%;
		}}
		.form input[type="submit"]:hover, .form button:hover {{
			background: var(--accent);
			transform: translateY(-3px);
		}}
		.response {{
			margin-top: 20px;
			padding: 15px;
			border-radius: 8px;
			background: rgba(39, 174, 96, 0.2);
			border: 1px solid #27ae60;
			color: #F0F5FF;
			text-align: left;
			font-size: 1.05em;
		}}
		@media (max-width: 600px) {{
			.container {{
				padding: 1rem;
			}}
			.feature-card {{
				padding: 1.1rem;
			}}
		}}
	</style>
</head>
<body>
	<div class="container">
		<div class="feature-card">
			<div class="logo-icon">🐍</div>
			<div class="logo-text">Python CGI Test</div>
			<div class="status-badge">Python CGI is working!</div>
			<div class="info-block">
				<p><strong>Server Software:</strong> {server_software}</p>
				<p><strong>Server Protocol:</strong> {server_protocol}</p>
				<p><strong>Request Method:</strong> {method}</p>
				<p><strong>Query String:</strong> {query_string}</p>
				<p><strong>Remote Address:</strong> {remote_addr}</p>
				<p><strong>User Agent:</strong> {user_agent}</p>
			</div>
			<div style="margin-top:2.2em;text-align:left;">
				<h3 style="margin-bottom:1em; color:var(--accent); font-size:1.15em;">Test Form</h3>
				<form class="form" method="POST">
					<input type="text" name="name" placeholder="Your Name" required>
					<input type="email" name="email" placeholder="Your Email" required>
					<textarea name="message" placeholder="Your Message" rows="4" required></textarea>
					<button type="submit">Submit</button>
				</form>
				{form_response}
			</div>
		</div>
	</div>
</body>
</html>
""")

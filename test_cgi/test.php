#!/opt/homebrew/bin/php-cgi
<?php
header("Content-Type: text/html; charset=UTF-8");
?>
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>PHP CGI Test</title>
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<style>
		/*style.css - A modern, responsive CSS stylesheet for a web page with a focus on dark mode and vibrant colors */
		:root {
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
		}
		* {
			margin: 0;
			padding: 0;
			box-sizing: border-box;
			font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
		}
		body {
			background: radial-gradient(ellipse at top, var(--darker), transparent),
				radial-gradient(ellipse at bottom, #1a1f3a, transparent),
				linear-gradient(to bottom, #0a0e17, #121826);
			color: var(--light);
			min-height: 100vh;
			overflow-x: hidden;
			line-height: 1.6;
			background-attachment: fixed;
		}
		body::before {
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
		}
		.container {
			max-width: 500px;
			margin: 7vh auto 0 auto;
			padding: 2rem;
		}
		.feature-card {
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
		}
		.feature-card:hover {
			transform: translateY(-10px) scale(1.02);
			box-shadow: 0 20px 40px rgba(0, 0, 0, 0.4);
			border-color: rgba(108, 99, 255, 0.3);
		}
		.logo-icon {
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
		}
		.logo-text {
			font-size: 1.8rem;
			font-weight: 700;
			background: linear-gradient(to right, var(--light), var(--accent));
			-webkit-background-clip: text;
			-webkit-text-fill-color: transparent;
			margin-bottom: 1.2rem;
		}
		.status-badge {
			margin-bottom: 1.5rem;
			display: inline-block;
			background: var(--primary);
			color: #fff;
			padding: 0.6em 1.5em;
			border-radius: 2em;
			font-weight: 600;
			font-size: 1.05em;
			box-shadow: 0 2px 8px rgba(108,99,255,0.12);
		}
		.info-block {
			text-align: left;
			margin: 1.8rem 0 0 0;
			background: rgba(255,255,255,0.03);
			padding: 1.2em 1em;
			border-radius: 10px;
			font-size: 1.08em;
		}
		.info-block p {
			margin: 10px 0;
			color: #F0F5FF;
		}
		@media (max-width: 600px) {
			.container {
				padding: 1rem;
			}
			.feature-card {
				padding: 1.1rem;
			}
		}
	</style>
</head>
<body>
	<div class="container">
		<div class="feature-card">
			<div class="logo-icon">🐘</div>
			<div class="logo-text">PHP CGI Test</div>
			<div class="status-badge">PHP is working!</div>
			<div class="info-block">
				<p><strong>PHP Version:</strong> <?php echo htmlspecialchars(phpversion()); ?></p>
				<p><strong>Request Method:</strong> <?php echo htmlspecialchars($_SERVER['REQUEST_METHOD']); ?></p>
				<p><strong>Query String:</strong> <?php echo htmlspecialchars($_SERVER['QUERY_STRING'] ?? ''); ?></p>
				<p><strong>Server Software:</strong> <?php echo htmlspecialchars($_SERVER['SERVER_SOFTWARE'] ?? ''); ?></p>
				<p><strong>Gateway Interface:</strong> <?php echo htmlspecialchars($_SERVER['GATEWAY_INTERFACE'] ?? ''); ?></p>
				<p><strong>Server Time:</strong> <?php echo date('Y-m-d H:i:s'); ?></p>
			</div>
		</div>
	</div>
</body>
</html>

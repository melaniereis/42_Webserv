#!/usr/bin/php
<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);
// Disable output buffering
@ini_set('output_buffering', 'Off');
@ini_set('implicit_flush', 1);
@ob_implicit_flush(true);
while (@ob_end_flush());

header("Content-Type: text/html; charset=UTF-8");

$cookieName = 'visit_count';
$counter = 1;
if (isset($_COOKIE[$cookieName]) && is_numeric($_COOKIE[$cookieName])) {
	$counter = (int)$_COOKIE[$cookieName] + 1;
}
setcookie(
	$cookieName,
	(string)$counter,
	time() + 365*24*60*60, // 1 year
	'/', '', false, true
);
$_COOKIE[$cookieName] = (string)$counter;
?>
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>PHP CGI Visit Counter</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
	:root { --primary:#6C63FF; --primary-dark:#5651d9; --secondary:#FF6584;
			--accent:#00C9A7; --dark:#121826; --darker:#0A0E17;
			--light:#F0F5FF; --card-bg:rgba(25,30,48,0.7);
			--transition:all .4s cubic-bezier(.175,.885,.32,1.275);
			--shadow:0 10px 30px rgba(0,0,0,0.3); }
	*{margin:0;padding:0;box-sizing:border-box;
	font-family:'Segoe UI',Tahoma,Verdana,sans-serif;}
	body{background:
	radial-gradient(ellipse at top,var(--darker),transparent),
	radial-gradient(ellipse at bottom,#1a1f3a,transparent),
	linear-gradient(to bottom,#0a0e17,#121826);
	color:var(--light);min-height:100vh;overflow-x:hidden;
	line-height:1.6;background-attachment:fixed;}
	body::before{content:"";position:fixed;top:0;left:0;width:100%;
	height:100%;background:
		radial-gradient(circle at 15% 30%,rgba(108,99,255,.08)0%,transparent25%),
		radial-gradient(circle at 85% 70%,rgba(255,101,132,.08)0%,transparent25%);
	z-index:-1;pointer-events:none;}
	.container{max-width:500px;margin:7vh auto;padding:2rem;}
	.feature-card{background:var(--card-bg);border-radius:20px;
	padding:2.5rem;transition:var(--transition);
	border:1px solid rgba(255,255,255,.08);backdrop-filter:blur(10px);
	text-align:center;position:relative;overflow:hidden;box-shadow:var(--shadow);}
	.feature-card:hover{transform:translateY(-10px) scale(1.02);
	box-shadow:0 20px 40px rgba(0,0,0,.4);
	border-color:rgba(108,99,255,.3);}
	.logo-icon{width:50px;height:50px;
	background:linear-gradient(135deg,var(--primary),var(--accent));
	border-radius:12px;display:flex;align-items:center;
	justify-content:center;font-size:2.3rem;color:#fff;margin:0 auto 1.2rem;
	box-shadow:var(--shadow);}
	.logo-text{font-size:1.8rem;font-weight:700;
	background:linear-gradient(to right,var(--light),var(--accent));
	-webkit-background-clip:text;-webkit-text-fill-color:transparent;
	margin-bottom:1.2rem;}
	.status-badge{display:inline-block;background:var(--primary);
	color:#fff;padding:.6em 1.5em;border-radius:2em;
	font-weight:600;font-size:1.05em;margin-bottom:1.5rem;
	box-shadow:0 2px 8px rgba(108,99,255,.12);}
	.info-block{text-align:left;margin:1.8rem 0;
	background:rgba(255,255,255,.03);padding:1.2em;border-radius:10px;
	font-size:1.08em;color:#F0F5FF;}
	.info-block p {margin:10px 0;}
	@media(max-width:600px){.container{padding:1rem;}
	.feature-card{padding:1.1rem;}}
</style>
</head>
<body>
<div class="container">
	<div class="feature-card">
	<div class="logo-icon">🐘</div>
	<div class="logo-text">PHP CGI Test</div>
	<div class="status-badge">PHP is working!</div>
	<div class="info-block">
		<p><strong>Visit Counter:</strong> <?php echo htmlspecialchars($counter); ?></p>
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

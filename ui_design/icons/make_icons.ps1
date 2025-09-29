param(
  [string] $OutputDir = ".\bmps",
  [int[]]  $Sizes = @(32,48),
  [ValidateSet(200,300,400,500,600,700)]
  [int]    $Weight = 400,
  [string] $Variant = "rounded",

  # One of these must be provided to locate resvg:
  [string] $ResvgPath = "",               # path to resvg.exe OR folder containing it
  [string] $ResvgZipUrl = "",             # direct ZIP URL of Windows resvg release (MSVC/x64)
  [string] $ResvgZipPath = "",            # local ZIP path of Windows resvg release

  # Colors
  [string] $IconColor = "#E5E7EB",        # light glyph color
  [string] $BackgroundColor = "#0F172A"   # dark surface background
)

# ---------------- Material Symbols (Rounded) logical -> candidates (fallbacks) ----------------
$IconMap = [ordered]@{
  # Global / navigation
  "home"                = @("home","home_pin","house");
  "debug"               = @("bug_report","terminal","bug_report_outline");

  # Home tiles
  "radios_tile"         = @("radio","radio_button_checked","settings_input_antenna");
  "autopilot_tile"      = @("explore","near_me","navigation");
  "simcontrols_tile"    = @("play_arrow","sports_esports","tune");
  "ground_tile"         = @("local_shipping","local_taxi","airport_shuttle");
  "engine_tile"         = @("speed","tire_repair","build_circle");
  "lights_tile"         = @("lightbulb","wb_incandescent","bolt");

  # Radios controls
  "radios_swap"         = @("swap_horiz","sync_alt","compare_arrows");
  "radios_plus"         = @("add","add_circle","add_box");
  "radios_minus"        = @("remove","remove_circle","indeterminate_check_box");

  # Sim Controls
  "sim_pause"           = @("pause","pause_circle","pause_presentation");
  "sim_active_pause"    = @("play_circle","play_arrow","play_circle_filled");
  "sim_rate_up"         = @("keyboard_double_arrow_up","arrow_upward","expand_less");
  "sim_rate_down"       = @("keyboard_double_arrow_down","arrow_downward","expand_more");

  # Ground / services
  "pushback_left"       = @("u_turn_left","turn_left","subdirectory_arrow_left");
  "pushback_right"      = @("u_turn_right","turn_right","subdirectory_arrow_right");
  "jetway"              = @("connecting_airports","bridge","lan");
  "stairs"              = @("stairs","stairs_outlined","escalator");
  "baggage"             = @("luggage","work","backpack");
  "fuel_service"        = @("local_gas_station","ev_station","propane_tank");

  # Engine / fuel
  "engine_fan"          = @("mode_fan","ac_unit","toys_fan");
  "engine_temp"         = @("device_thermostat","thermostat","thermostat_auto");
  "fuel"                = @("water_drop","opacity","droplet");
  "crossfeed"           = @("swap_vert","import_export","swap_vert_circle");

  # Autopilot
  "ap_nav"              = @("navigation","near_me","explore");
  "ap_alt_hold"         = @("vertical_align_bottom","height","north");
  "ap_vs"               = @("trending_up","show_chart","stacked_line_chart");
  "ap_hdg"              = @("explore","navigation","near_me");

  # Ground/taxi/road & lights
  "taxi_guidance"       = @("signpost","assistant_direction","map");
  "landing_light"       = @("wb_sunny","light_mode","sunny");
  "strobe_light"        = @("flash_on","bolt","auto_fix_high");

  # Dashboard/panel
  "panel"               = @("dashboard","space_dashboard","analytics");
}

# ---------------- Paths ----------------
$Base    = (Get-Location).Path
$Work    = Join-Path $Base "material_symbols_work"
$SvgsDir = Join-Path $Work "input_svgs"
$PngDir  = Join-Path $Work "temp_png"
$Tools   = Join-Path $Base "tools"
$ResvgDir= Join-Path $Tools "resvg"
$OutDir  = Resolve-Path -LiteralPath $OutputDir -ErrorAction SilentlyContinue
if (-not $OutDir) { New-Item -ItemType Directory -Path $OutputDir | Out-Null; $OutDir = Resolve-Path $OutputDir }

# ---------------- Helpers ----------------
function New-Dirs([string]$dir) { if (-not (Test-Path $dir)) { New-Item -ItemType Directory -Path $dir | Out-Null } }

function Download-File($Url, $OutPath) {
  try { [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12 } catch {}
  Invoke-WebRequest -Uri $Url -OutFile $OutPath -UseBasicParsing -ErrorAction Stop | Out-Null
}

function Fix-HexColor([string]$c) {
  if ($c -match '^#[0-9A-Fa-f]{3}$') { return "#{0}{0}{1}{1}{2}{2}" -f $c[1], $c[2], $c[3] }
  return $c
}

function Recolor-SVG([string]$svgPath, [string]$iconHex) {
  $iconHex = Fix-HexColor $iconHex

  # Load as XML (preserves structure; avoids malformed markup)
  [xml]$xml = Get-Content -LiteralPath $svgPath -Raw
  if (-not $xml.svg) {
    # If parsing fails (non-XML), fallback to minimal safe replacement
    $raw = Get-Content -LiteralPath $svgPath -Raw
    $raw = $raw -replace 'fill\s*=\s*["'']currentColor["'']', "fill=""$iconHex"""
    $raw = $raw -replace 'stroke\s*=\s*["'']currentColor["'']', "stroke=""$iconHex"""
    # Add root color to help currentColor resolve
    if ($raw -notmatch '<svg[^>]*\scolor=') {
      $raw = $raw -replace '<svg(\s+[^>]*)>', "<svg`$1 color=""$iconHex"">"
    }
    Set-Content -LiteralPath $svgPath -Value $raw -Encoding UTF8
    return
  }

  # Ensure root color so `currentColor` resolves
  $xml.svg.SetAttribute("color", $iconHex) | Out-Null

  # Helper to parse and rebuild the 'style' attribute safely
  function Parse-Style([string]$style) {
    $d = @{}
    if ([string]::IsNullOrWhiteSpace($style)) { return $d }
    foreach ($pair in $style.Split(';')) {
      if ($pair -match '^\s*([^:]+)\s*:\s*(.+)\s*$') {
        $k = $matches[1].Trim().ToLowerInvariant()
        $v = $matches[2].Trim()
        if ($k) { $d[$k] = $v }
      }
    }
    return $d
  }
  function Build-Style($dict) {
    if (-not $dict -or $dict.Count -eq 0) { return $null }
    ($dict.GetEnumerator() | ForEach-Object { "$($_.Key):$($_.Value)" }) -join ';'
  }

  # Elements we want to enforce color on
  $targets = @("path","rect","circle","ellipse","polygon","polyline","line","g")

  # Walk all descendants
  $nodes = $xml.svg.SelectNodes(".//*")
  foreach ($n in $nodes) {
    if ($targets -notcontains $n.Name) { continue }

    # Normalize/clean style
    $styleDict = Parse-Style($n.GetAttribute("style"))
    foreach ($k in @("color","opacity","fill-opacity","stroke-opacity")) {
      if ($styleDict.ContainsKey($k)) { $styleDict.Remove($k) }
    }

    # If explicit currentColor/black in attributes, override them
    $fill   = $n.GetAttribute("fill")
    $stroke = $n.GetAttribute("stroke")

    if ($fill -match '^(?:currentColor|#000000|black)$') { $fill = $iconHex }
    if ($stroke -match '^(?:currentColor|#000000|black)$') { $stroke = $iconHex }

    # If neither fill nor stroke set, enforce both to icon color (covers outline and filled glyphs)
    if ([string]::IsNullOrWhiteSpace($fill))   { $fill   = $iconHex }
    if ([string]::IsNullOrWhiteSpace($stroke)) { $stroke = $iconHex }

    $n.SetAttribute("fill",   $fill)   | Out-Null
    $n.SetAttribute("stroke", $stroke) | Out-Null

    # Ensure fully opaque
    if (-not $n.HasAttribute("fill-opacity"))   { $n.SetAttribute("fill-opacity",   "1") | Out-Null }
    if (-not $n.HasAttribute("stroke-opacity")) { $n.SetAttribute("stroke-opacity", "1") | Out-Null }

    # Rebuild style (without conflicting keys)
    $newStyle = Build-Style $styleDict
    if ($newStyle) { $n.SetAttribute("style", $newStyle) | Out-Null }
    else {
      if ($n.HasAttribute("style")) { $n.RemoveAttribute("style") | Out-Null }
    }
  }

  # Save as UTF-8 without BOM
  $sw = New-Object System.IO.StreamWriter($svgPath, $false, (New-Object System.Text.UTF8Encoding($false)))
  $xml.Save($sw)
  $sw.Close()
}

function Resolve-Resvg {
  param([string]$ResvgPathP, [string]$ZipUrl, [string]$ZipPath)

  function _EnsureExe([string]$p) {
    if ((Test-Path $p) -and (Get-Item $p).PSIsContainer) { $p = Join-Path $p "resvg.exe" }
    if (-not (Test-Path $p)) { throw "resvg.exe not found at: $p" }
    return (Resolve-Path $p).Path
  }

  if ($ResvgPathP) { return (_EnsureExe $ResvgPathP) }

  New-Dirs $Tools; New-Dirs $ResvgDir
  $localExe = Join-Path $ResvgDir "resvg.exe"
  if (Test-Path $localExe) { return (Resolve-Path $localExe).Path }

  if ($ZipUrl) {
    $tmp = Join-Path $ResvgDir "resvg.zip"
    Write-Host "==> Downloading resvg ZIP from provided URL..."
    Download-File $ZipUrl $tmp
    Write-Host "==> Extracting resvg..."
    Expand-Archive -Path $tmp -DestinationPath $ResvgDir -Force
    Remove-Item $tmp -Force -ErrorAction SilentlyContinue
    $exe = Get-ChildItem -Path $ResvgDir -Recurse -Filter "resvg.exe" | Select-Object -First 1
    if (-not $exe) { throw "ZIP downloaded but resvg.exe not found inside." }
    return $exe.FullName
  }

  if ($ZipPath) {
    if (-not (Test-Path $ZipPath)) { throw "Provided -ResvgZipPath not found: $ZipPath" }
    Write-Host "==> Extracting resvg from provided ZIP..."
    Expand-Archive -Path $ZipPath -DestinationPath $ResvgDir -Force
    $exe = Get-ChildItem -Path $ResvgDir -Recurse -Filter "resvg.exe" | Select-Object -First 1
    if (-not $exe) { throw "resvg.exe not found inside ZIP: $ZipPath" }
    return $exe.FullName
  }

  throw "No resvg specified. Use -ResvgPath (path or folder), -ResvgZipUrl, or -ResvgZipPath."
}

function Download-SVG([string]$icon, [string]$destPath, [int]$weight, [string]$variant) {
  $url = "https://unpkg.com/@material-symbols/svg-$weight/$variant/$icon.svg"
  try { Download-File $url $destPath; return $true } catch { return $false }
}

function Export-PNG-Resvg([string]$resvgExe, [string]$svgPath, [string]$pngPath, [int]$size, [string]$bg) {
  $bg = Fix-HexColor $bg
  $args = @("--background", "$bg", "-w", "$size", "-h", "$size", "$svgPath", "$pngPath")
  & "$resvgExe" @args | Out-Null
  if ($LASTEXITCODE -eq 0 -or $LASTEXITCODE -eq $null) { return $true }
  return $false
}

function Convert-PNG-To-BMP24([string]$pngPath, [string]$bmpPath) {
  Add-Type -AssemblyName System.Drawing
  $bmpDir = Split-Path -Parent $bmpPath
  if (-not (Test-Path $bmpDir)) { New-Dirs $bmpDir }

  $img = [System.Drawing.Image]::FromFile($pngPath)
  try {
    $bmp24 = New-Object System.Drawing.Bitmap($img.Width, $img.Height, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
    $g = [System.Drawing.Graphics]::FromImage($bmp24)
    try { $g.DrawImage($img, 0, 0, $img.Width, $img.Height) } finally { $g.Dispose() }
    $bmp24.Save($bmpPath, [System.Drawing.Imaging.ImageFormat]::Bmp)
  } finally {
    $img.Dispose()
    Remove-Item -LiteralPath $pngPath -ErrorAction SilentlyContinue
  }
}

# ---------------- Prep ----------------
New-Dirs $Work; New-Dirs $SvgsDir; New-Dirs $PngDir; New-Dirs $OutDir.Path

$resvgExe = $null
try { $resvgExe = Resolve-Resvg -ResvgPathP $ResvgPath -ZipUrl $ResvgZipUrl -ZipPath $ResvgZipPath }
catch { Write-Error $_.Exception.Message; exit 1 }
Write-Host "==> Using resvg: $resvgExe"

# ---------------- Download icons with fallbacks (and recolor) ----------------
$Chosen  = @{}
$Missing = @()
Write-Host "==> Downloading Material Symbols (Rounded, weight $Weight) with fallbacks..."
foreach ($logical in $IconMap.Keys) {
  $picked = $null
  foreach ($name in $IconMap[$logical]) {
    $svgPath = Join-Path $SvgsDir "$name.svg"
    if (Test-Path $svgPath) {
      Recolor-SVG -svgPath $svgPath -iconHex $IconColor
      $picked = $name; break
    }
    if (Download-SVG -icon $name -destPath $svgPath -weight $Weight -variant $Variant) {
      Recolor-SVG -svgPath $svgPath -iconHex $IconColor
      $picked = $name; break
    }
  }
  if ($picked) { $Chosen[$logical] = $picked; Write-Host "  [OK] $logical -> $picked.svg" }
  else { $Missing += $logical; Write-Warning "  [MISS] $logical (no candidates at this weight/variant)" }
}
if ($Chosen.Count -eq 0) { Write-Error "No icons downloaded."; exit 2 }

# ---------------- Convert: SVG -> PNG (resvg) -> BMP (24-bit) ----------------
Write-Host "==> Converting to 24-bit BMP; sizes: $($Sizes -join ', ')"
foreach ($kv in $Chosen.GetEnumerator()) {
  $logical = $kv.Key; $actual = $kv.Value
  $svgPath = Join-Path $SvgsDir "$actual.svg"
  foreach ($size in $Sizes) {
    $pngPath = Join-Path $PngDir  ("{0}_{1}px.png" -f $logical, $size)
    $bmpPath = Join-Path $OutDir   ("{0}_{1}px.bmp" -f $logical, $size)
    $ok = Export-PNG-Resvg -resvgExe $resvgExe -svgPath $svgPath -pngPath $pngPath -size $size -bg $BackgroundColor
    if (-not $ok) { Write-Warning "  [FAIL] $logical @ ${size}px (resvg)"; continue }
    Convert-PNG-To-BMP24 -pngPath $pngPath -bmpPath $bmpPath
    Write-Host "  [OK] $([System.IO.Path]::GetFileName($bmpPath))  (glyph: $actual)"
  }
}

# ---------------- Manifest ----------------
$csvPath = Join-Path $OutDir "icon_manifest.csv"
"Logical,MaterialGlyph,Weight,Variant,IconColor,BackgroundColor" | Out-File -FilePath $csvPath -Encoding UTF8
foreach ($kv in $Chosen.GetEnumerator()) {
  "$($kv.Key),$($kv.Value),$Weight,$Variant,$IconColor,$BackgroundColor" | Out-File -FilePath $csvPath -Append -Encoding UTF8
}
Write-Host "`n==> DONE. Saved BMPs to: $($OutDir.Path)"
Write-Host "Manifest: $csvPath"
if ($Missing.Count -gt 0) {
  Write-Host "`nMissing logical icons:"
  $Missing | ForEach-Object { Write-Host "  - $_" }
  Write-Host "Try a different -Weight (e.g., 500) or adjust candidate names."
}

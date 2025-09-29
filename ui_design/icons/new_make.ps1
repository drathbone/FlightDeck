# one-shot wrapper
$Resvg = "C:\Users\Darren\Desktop\FlightDeck\UI Design\icons\resvg.exe"
$Sizes = @(32,48)
$Weight = 400
$BG = "#0F172A"

$themes = @{
  "bmps_gray"  = "#E5E7EB"
  "bmps_cyan"  = "#22D3EE"
  "bmps_amber" = "#F59E0B"
  "bmps_red"   = "#EF4444"
}

foreach ($dir in $themes.Keys) {
  .\make_icons.ps1 -ResvgPath "$Resvg" -OutputDir ".\$dir" -Sizes $Sizes -Weight $Weight `
    -IconColor $themes[$dir] -BackgroundColor $BG
}

param(
    [string]$PlantsDir = "assets/texture/Plants",
    [string]$OutputDir = "assets/texture/Misc/PlantIcons",
    [string]$PacketOutputDir = "assets/texture/Misc/PlantSeedPackets",
    [string]$SeedPacketPath = "assets/texture/Misc/PlantChooser/SeedPacket_Larger.png",
    [int]$IconSize = 128,
    [int]$Padding = 12,
    [int]$FrameIndex = -1,
    [string[]]$PlantNames = @()
)

Add-Type -AssemblyName System.Drawing

function Get-TagText {
    param($Node, [string]$Name)

    if ($null -eq $Node -or -not ($Node -is [System.Xml.XmlNode])) {
        return $null
    }

    $child = $Node.SelectSingleNode($Name)
    if ($null -eq $child) {
        return $null
    }
    return $child.InnerText
}

function Get-ImageKey {
    param([string]$ImageName)

    if ([string]::IsNullOrWhiteSpace($ImageName)) {
        return ""
    }

    $key = $ImageName.Trim()
    if ($key.StartsWith("IMAGE_REANIM_")) {
        $key = $key.Substring("IMAGE_REANIM_".Length)
    }
    return $key.ToUpperInvariant()
}

function Get-FloatTag {
    param($Node, [string]$Name, [float]$CurrentValue)

    $text = Get-TagText $Node $Name
    if ([string]::IsNullOrWhiteSpace($text)) {
        return $CurrentValue
    }

    return [float]::Parse($text, [Globalization.CultureInfo]::InvariantCulture)
}

function Get-ReanimFile {
    param([System.IO.DirectoryInfo]$PlantDir)

    $reanimOverrides = @{
        PeaShooter = "PeaShooterSingle.reanim"
    }

    if ($reanimOverrides.ContainsKey($PlantDir.Name)) {
        $override = Join-Path $PlantDir.FullName $reanimOverrides[$PlantDir.Name]
        if (Test-Path $override) {
            return Get-Item $override
        }
    }

    $preferred = Join-Path $PlantDir.FullName ($PlantDir.Name + ".reanim")
    if (Test-Path $preferred) {
        return Get-Item $preferred
    }

    return Get-ChildItem -LiteralPath $PlantDir.FullName -Filter "*.reanim" |
        Sort-Object Name |
        Select-Object -First 1
}

function Read-DrawItems {
    param(
        [System.IO.FileInfo]$ReanimFile,
        [hashtable]$ImageFiles,
        [int]$TargetFrame
    )

    $raw = Get-Content -LiteralPath $ReanimFile.FullName -Raw
    [xml]$doc = "<reanim>$raw</reanim>"
    $items = New-Object System.Collections.Generic.List[object]

    foreach ($track in $doc.SelectNodes("/reanim/track")) {
        $frames = @($track.SelectNodes("t"))
        if ($frames.Count -eq 0) {
            continue
        }

        [float]$x = 0.0
        [float]$y = 0.0
        [float]$sx = 1.0
        [float]$sy = 1.0
        [float]$kx = 0.0
        [float]$ky = 0.0
        [float]$alpha = 1.0
        [string]$imageName = ""

        $lastIndex = [Math]::Min($TargetFrame, $frames.Count - 1)
        for ($i = 0; $i -le $lastIndex; $i++) {
            $frame = $frames[$i]

            $x = Get-FloatTag $frame "x" $x
            $y = Get-FloatTag $frame "y" $y
            $sx = Get-FloatTag $frame "sx" $sx
            $sy = Get-FloatTag $frame "sy" $sy
            $kx = Get-FloatTag $frame "kx" $kx
            $ky = Get-FloatTag $frame "ky" $ky

            $fText = Get-TagText $frame "f"
            if (-not [string]::IsNullOrWhiteSpace($fText)) {
                $fValue = [int]::Parse($fText, [Globalization.CultureInfo]::InvariantCulture)
                if ($fValue -eq -1) {
                    $alpha = 0.0
                } elseif ($fValue -eq 0) {
                    $alpha = 1.0
                } else {
                    $alpha = [Math]::Min(1.0, $fValue / 255.0)
                }
            }

            $iText = Get-TagText $frame "i"
            if (-not [string]::IsNullOrWhiteSpace($iText)) {
                $imageName = $iText
            }
        }

        $imageKey = Get-ImageKey $imageName
        if ($alpha -le 0.0 -or [string]::IsNullOrWhiteSpace($imageKey) -or -not $ImageFiles.ContainsKey($imageKey)) {
            continue
        }

        $items.Add([pscustomobject]@{
            ImagePath = $ImageFiles[$imageKey]
            ImageKey = $imageKey
            X = $x
            Y = $y
            ScaleX = $sx
            ScaleY = $sy
            SkewX = $kx
            SkewY = $ky
            Alpha = $alpha
        })
    }

    return $items
}

function Read-CompositeDrawItems {
    param(
        [System.IO.FileInfo]$ReanimFile,
        [hashtable]$ImageFiles
    )

    $raw = Get-Content -LiteralPath $ReanimFile.FullName -Raw
    [xml]$doc = "<reanim>$raw</reanim>"
    $items = New-Object System.Collections.Generic.List[object]

    foreach ($track in $doc.SelectNodes("/reanim/track")) {
        $frames = @($track.SelectNodes("t"))
        if ($frames.Count -eq 0) {
            continue
        }

        [float]$x = 0.0
        [float]$y = 0.0
        [float]$sx = 1.0
        [float]$sy = 1.0
        [float]$kx = 0.0
        [float]$ky = 0.0
        [float]$alpha = 1.0
        [string]$imageName = ""

        foreach ($frame in $frames) {
            $x = Get-FloatTag $frame "x" $x
            $y = Get-FloatTag $frame "y" $y
            $sx = Get-FloatTag $frame "sx" $sx
            $sy = Get-FloatTag $frame "sy" $sy
            $kx = Get-FloatTag $frame "kx" $kx
            $ky = Get-FloatTag $frame "ky" $ky

            $fText = Get-TagText $frame "f"
            if (-not [string]::IsNullOrWhiteSpace($fText)) {
                $fValue = [int]::Parse($fText, [Globalization.CultureInfo]::InvariantCulture)
                if ($fValue -eq -1) {
                    $alpha = 0.0
                } elseif ($fValue -eq 0) {
                    $alpha = 1.0
                } else {
                    $alpha = [Math]::Min(1.0, $fValue / 255.0)
                }
            }

            $iText = Get-TagText $frame "i"
            if (-not [string]::IsNullOrWhiteSpace($iText)) {
                $imageName = $iText
            }

            $imageKey = Get-ImageKey $imageName
            if ($alpha -gt 0.0 -and -not [string]::IsNullOrWhiteSpace($imageKey) -and $ImageFiles.ContainsKey($imageKey)) {
                $items.Add([pscustomobject]@{
                    ImagePath = $ImageFiles[$imageKey]
                    ImageKey = $imageKey
                    X = $x
                    Y = $y
                    ScaleX = $sx
                    ScaleY = $sy
                    SkewX = $kx
                    SkewY = $ky
                    Alpha = $alpha
                })
                break
            }
        }
    }

    return $items
}

function Get-Clips {
    param([System.IO.FileInfo]$ReanimFile)

    $raw = Get-Content -LiteralPath $ReanimFile.FullName -Raw
    [xml]$doc = "<reanim>$raw</reanim>"
    $clips = New-Object System.Collections.Generic.List[object]

    foreach ($track in $doc.SelectNodes("/reanim/track")) {
        $nameNode = $track.SelectSingleNode("name")
        if ($null -eq $nameNode -or -not $nameNode.InnerText.StartsWith("anim_")) {
            continue
        }

        $clipName = $nameNode.InnerText.Substring("anim_".Length)
        $frames = @($track.SelectNodes("t"))
        if ($frames.Count -eq 0) {
            continue
        }

        [float]$alpha = 1.0
        $inVisible = $false
        $start = 0

        for ($i = 0; $i -lt $frames.Count; $i++) {
            $fText = Get-TagText $frames[$i] "f"
            if (-not [string]::IsNullOrWhiteSpace($fText)) {
                $fValue = [int]::Parse($fText, [Globalization.CultureInfo]::InvariantCulture)
                if ($fValue -eq -1) {
                    $alpha = 0.0
                } elseif ($fValue -eq 0) {
                    $alpha = 1.0
                } else {
                    $alpha = [Math]::Min(1.0, $fValue / 255.0)
                }
            }

            if (-not $inVisible -and $alpha -gt 0.0) {
                $inVisible = $true
                $start = $i
            } elseif ($inVisible -and $alpha -le 0.0) {
                $end = [Math]::Max($start, $i - 1)
                $clips.Add([pscustomobject]@{
                    Name = $clipName
                    Start = $start
                    End = $end
                    Length = $end - $start + 1
                })
                $inVisible = $false
            }
        }

        if ($inVisible) {
            $end = $frames.Count - 1
            $clips.Add([pscustomobject]@{
                Name = $clipName
                Start = $start
                End = $end
                Length = $end - $start + 1
            })
        }
    }

    return $clips
}

function Select-IconClip {
    param($Clips)

    if ($null -eq $Clips -or $Clips.Count -eq 0) {
        return $null
    }

    $preferred = @("full_idle", "idle", "armed", "scaredidle", "head_idle")
    foreach ($name in $preferred) {
        $match = @($Clips | Where-Object { $_.Name -eq $name -and $_.Length -ge 3 } | Sort-Object Length -Descending | Select-Object -First 1)
        if ($match.Count -gt 0) {
            return $match[0]
        }
    }

    foreach ($name in $preferred) {
        $match = @($Clips | Where-Object { $_.Name -eq $name -and $_.Length -ge 1 } | Sort-Object Length -Descending | Select-Object -First 1)
        if ($match.Count -gt 0) {
            return $match[0]
        }
    }

    $fallback = @($Clips |
        Where-Object { $_.Length -ge 3 -and $_.Name -notmatch "blink|shoot|mashed|rise|grow|sleep|glow" } |
        Sort-Object Length -Descending |
        Select-Object -First 1)

    if ($fallback.Count -gt 0) {
        return $fallback[0]
    }

    return ($Clips | Sort-Object Length -Descending | Select-Object -First 1)
}

function Select-BestFrameInClip {
    param(
        [System.IO.FileInfo]$ReanimFile,
        [hashtable]$ImageFiles,
        $Clip
    )

    $bestFrame = $Clip.Start
    $bestScore = [double]::NegativeInfinity
    $samples = New-Object System.Collections.Generic.List[int]

    for ($frame = $Clip.Start; $frame -le [Math]::Min($Clip.End, $Clip.Start + 8); $frame++) {
        $samples.Add($frame)
    }
    $samples.Add([int][Math]::Floor(($Clip.Start + $Clip.End) / 2))
    $samples.Add($Clip.End)

    foreach ($frame in ($samples | Select-Object -Unique | Sort-Object)) {
        $items = Read-DrawItems $ReanimFile $ImageFiles $frame
        if ($items.Count -eq 0) {
            continue
        }

        $badCount = @($items | Where-Object { $_.ImageKey -match "BLINK2|MASHED|LIGHT|GLOW" }).Count
        $score = $items.Count * 100 - $badCount * 1000

        if ($score -gt $bestScore) {
            $bestScore = $score
            $bestFrame = $frame
        }
    }

    return $bestFrame
}

function Get-AutoFrameIndex {
    param(
        [System.IO.FileInfo]$ReanimFile,
        [hashtable]$ImageFiles
    )

    $raw = Get-Content -LiteralPath $ReanimFile.FullName -Raw
    [xml]$doc = "<reanim>$raw</reanim>"
    $counts = @{}
    $tracks = @($doc.SelectNodes("/reanim/track"))
    $maxFrameCount = 0

    foreach ($track in $tracks) {
        $frames = @($track.SelectNodes("t"))
        if ($frames.Count -gt $maxFrameCount) {
            $maxFrameCount = $frames.Count
        }
    }

    if ($maxFrameCount -eq 0) {
        return 0
    }

    foreach ($track in $tracks) {
        $frames = @($track.SelectNodes("t"))
        [float]$alpha = 1.0
        [string]$imageName = ""

        for ($i = 0; $i -lt $maxFrameCount; $i++) {
            if ($i -lt $frames.Count) {
                $frame = $frames[$i]

                $fText = Get-TagText $frame "f"
                if (-not [string]::IsNullOrWhiteSpace($fText)) {
                    $fValue = [int]::Parse($fText, [Globalization.CultureInfo]::InvariantCulture)
                    if ($fValue -eq -1) {
                        $alpha = 0.0
                    } elseif ($fValue -eq 0) {
                        $alpha = 1.0
                    } else {
                        $alpha = [Math]::Min(1.0, $fValue / 255.0)
                    }
                }

                $iText = Get-TagText $frame "i"
                if (-not [string]::IsNullOrWhiteSpace($iText)) {
                    $imageName = $iText
                }
            }

            $imageKey = Get-ImageKey $imageName
            if ($alpha -gt 0.0 -and -not [string]::IsNullOrWhiteSpace($imageKey) -and $ImageFiles.ContainsKey($imageKey)) {
                $key = [string]$i
                if (-not $counts.ContainsKey($key)) {
                    $counts[$key] = 0
                }
                $counts[$key]++
            }
        }
    }

    if ($counts.Count -eq 0) {
        return 0
    }

    $best = $counts.GetEnumerator() |
        Sort-Object @{ Expression = "Value"; Descending = $true }, @{ Expression = { [int]$_.Key }; Ascending = $true } |
        Select-Object -First 1

    return [int]$best.Key
}

function Add-Corner {
    param(
        [System.Collections.Generic.List[float]]$Xs,
        [System.Collections.Generic.List[float]]$Ys,
        [float]$X,
        [float]$Y,
        [float]$A,
        [float]$B,
        [float]$C,
        [float]$D,
        [float]$Tx,
        [float]$Ty
    )

    $Xs.Add($A * $X + $C * $Y + $Tx)
    $Ys.Add($B * $X + $D * $Y + $Ty)
}

function Draw-ImagePart {
    param(
        [System.Drawing.Graphics]$Graphics,
        [System.Drawing.Image]$Image,
        [int]$X,
        [int]$Y,
        [int]$Width,
        [int]$Height
    )

    $src = New-Object System.Drawing.Rectangle($X, $Y, $Width, $Height)
    $dst = New-Object System.Drawing.Rectangle($X, $Y, $Width, $Height)
    $Graphics.DrawImage($Image, $dst, $src, [System.Drawing.GraphicsUnit]::Pixel)
}

function Export-SeedPacketIcon {
    param(
        [System.Drawing.Bitmap]$PlantIcon,
        [System.Drawing.Image]$SeedPacket,
        [string]$OutputPath
    )

    $packetBitmap = New-Object System.Drawing.Bitmap($SeedPacket.Width, $SeedPacket.Height, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $packetGraphics = [System.Drawing.Graphics]::FromImage($packetBitmap)
    $packetGraphics.Clear([System.Drawing.Color]::Transparent)
    $packetGraphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $packetGraphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality

    $packetGraphics.DrawImage($SeedPacket, 0, 0, $SeedPacket.Width, $SeedPacket.Height)

    $plantArea = New-Object System.Drawing.Rectangle(10, 30, 80, 68)
    $fitScale = [Math]::Min($plantArea.Width / $PlantIcon.Width, $plantArea.Height / $PlantIcon.Height)
    $plantWidth = [int][Math]::Round($PlantIcon.Width * $fitScale)
    $plantHeight = [int][Math]::Round($PlantIcon.Height * $fitScale)
    $plantX = [int][Math]::Round($plantArea.X + ($plantArea.Width - $plantWidth) / 2)
    $plantY = [int][Math]::Round($plantArea.Y + ($plantArea.Height - $plantHeight) / 2)

    $packetGraphics.DrawImage($PlantIcon, $plantX, $plantY, $plantWidth, $plantHeight)

    Draw-ImagePart $packetGraphics $SeedPacket 0 0 $SeedPacket.Width 30
    Draw-ImagePart $packetGraphics $SeedPacket 0 30 9 72
    Draw-ImagePart $packetGraphics $SeedPacket 91 30 9 72
    Draw-ImagePart $packetGraphics $SeedPacket 0 98 $SeedPacket.Width ($SeedPacket.Height - 98)

    $packetGraphics.Dispose()
    $packetBitmap.Save($OutputPath, [System.Drawing.Imaging.ImageFormat]::Png)
    $packetBitmap.Dispose()
}

function Export-PlantIcon {
    param(
        [System.IO.DirectoryInfo]$PlantDir,
        [System.IO.FileInfo]$ReanimFile,
        [string]$OutputPath
    )

    $imageFiles = @{}
    foreach ($file in Get-ChildItem -LiteralPath $PlantDir.FullName -Filter "*.png") {
        $imageFiles[$file.BaseName.ToUpperInvariant()] = $file.FullName
    }

    if ($FrameIndex -lt 0 -and @("ThreePeater", "GatlingPea") -contains $PlantDir.Name) {
        $items = Read-CompositeDrawItems $ReanimFile $imageFiles
    } elseif ($FrameIndex -lt 0) {
        $clip = Select-IconClip (Get-Clips $ReanimFile)
        if ($null -ne $clip) {
            $targetFrame = Select-BestFrameInClip $ReanimFile $imageFiles $clip
            $items = Read-DrawItems $ReanimFile $imageFiles $targetFrame
        } else {
            $items = Read-CompositeDrawItems $ReanimFile $imageFiles
        }
    } else {
        $items = Read-DrawItems $ReanimFile $imageFiles $FrameIndex
    }
    if ($items.Count -eq 0) {
        Write-Warning "No drawable frame found for $($PlantDir.Name)"
        return $false
    }

    $loadedImages = @{}
    $xs = New-Object System.Collections.Generic.List[float]
    $ys = New-Object System.Collections.Generic.List[float]

    foreach ($item in $items) {
        if (-not $loadedImages.ContainsKey($item.ImagePath)) {
            $loadedImages[$item.ImagePath] = [System.Drawing.Image]::FromFile($item.ImagePath)
        }

        $img = $loadedImages[$item.ImagePath]
        $kx = $item.SkewX * [Math]::PI / 180.0
        $ky = $item.SkewY * [Math]::PI / 180.0
        $a = $item.ScaleX * [Math]::Cos($kx)
        $b = $item.ScaleX * [Math]::Sin($kx)
        $c = -$item.ScaleY * [Math]::Sin($ky)
        $d = $item.ScaleY * [Math]::Cos($ky)

        Add-Corner $xs $ys 0 0 $a $b $c $d $item.X $item.Y
        Add-Corner $xs $ys $img.Width 0 $a $b $c $d $item.X $item.Y
        Add-Corner $xs $ys 0 $img.Height $a $b $c $d $item.X $item.Y
        Add-Corner $xs $ys $img.Width $img.Height $a $b $c $d $item.X $item.Y
    }

    $minX = ($xs | Measure-Object -Minimum).Minimum
    $maxX = ($xs | Measure-Object -Maximum).Maximum
    $minY = ($ys | Measure-Object -Minimum).Minimum
    $maxY = ($ys | Measure-Object -Maximum).Maximum

    $rawWidth = [Math]::Max(1, [int][Math]::Ceiling($maxX - $minX))
    $rawHeight = [Math]::Max(1, [int][Math]::Ceiling($maxY - $minY))

    $rawBitmap = New-Object System.Drawing.Bitmap($rawWidth, $rawHeight, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $rawGraphics = [System.Drawing.Graphics]::FromImage($rawBitmap)
    $rawGraphics.Clear([System.Drawing.Color]::Transparent)
    $rawGraphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $rawGraphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality

    foreach ($item in $items) {
        $img = $loadedImages[$item.ImagePath]
        $kx = $item.SkewX * [Math]::PI / 180.0
        $ky = $item.SkewY * [Math]::PI / 180.0
        $a = $item.ScaleX * [Math]::Cos($kx)
        $b = $item.ScaleX * [Math]::Sin($kx)
        $c = -$item.ScaleY * [Math]::Sin($ky)
        $d = $item.ScaleY * [Math]::Cos($ky)

        $matrix = New-Object System.Drawing.Drawing2D.Matrix(
            [float]$a,
            [float]$b,
            [float]$c,
            [float]$d,
            [float]($item.X - $minX),
            [float]($item.Y - $minY)
        )
        $rawGraphics.Transform = $matrix
        $rawGraphics.DrawImage($img, 0, 0, $img.Width, $img.Height)
    }

    $rawGraphics.ResetTransform()
    $rawGraphics.Dispose()

    $finalBitmap = New-Object System.Drawing.Bitmap($IconSize, $IconSize, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $finalGraphics = [System.Drawing.Graphics]::FromImage($finalBitmap)
    $finalGraphics.Clear([System.Drawing.Color]::Transparent)
    $finalGraphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $finalGraphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality

    $available = [Math]::Max(1, $IconSize - 2 * $Padding)
    $fitScale = [Math]::Min($available / $rawWidth, $available / $rawHeight)
    $drawWidth = [int][Math]::Round($rawWidth * $fitScale)
    $drawHeight = [int][Math]::Round($rawHeight * $fitScale)
    $drawX = [int][Math]::Round(($IconSize - $drawWidth) / 2)
    $drawY = [int][Math]::Round(($IconSize - $drawHeight) / 2)

    $finalGraphics.DrawImage($rawBitmap, $drawX, $drawY, $drawWidth, $drawHeight)
    $finalGraphics.Dispose()

    $finalBitmap.Save($OutputPath, [System.Drawing.Imaging.ImageFormat]::Png)

    $rawBitmap.Dispose()
    $finalBitmap.Dispose()
    foreach ($img in $loadedImages.Values) {
        $img.Dispose()
    }

    return $true
}

$root = Resolve-Path "."
$plantsPath = Join-Path $root $PlantsDir
$outputPath = Join-Path $root $OutputDir
$packetOutputPath = Join-Path $root $PacketOutputDir
$seedPacketFullPath = Join-Path $root $SeedPacketPath
New-Item -ItemType Directory -Force -Path $outputPath | Out-Null
New-Item -ItemType Directory -Force -Path $packetOutputPath | Out-Null

$seedPacketImage = $null
if (Test-Path $seedPacketFullPath) {
    $seedPacketImage = [System.Drawing.Image]::FromFile($seedPacketFullPath)
} else {
    Write-Warning "Seed packet texture not found: $seedPacketFullPath"
}

$plantDirs = Get-ChildItem -LiteralPath $plantsPath -Directory | Sort-Object Name
if ($PlantNames.Count -gt 0) {
    $wanted = @{}
    foreach ($name in $PlantNames) {
        $wanted[$name.ToLowerInvariant()] = $true
    }
    $plantDirs = $plantDirs | Where-Object { $wanted.ContainsKey($_.Name.ToLowerInvariant()) }
}

foreach ($plantDir in $plantDirs) {
    $reanim = Get-ReanimFile $plantDir
    if ($null -eq $reanim) {
        Write-Warning "No reanim found for $($plantDir.Name)"
        continue
    }

    $iconPath = Join-Path $outputPath ($plantDir.Name + ".png")
    if (Export-PlantIcon $plantDir $reanim $iconPath) {
        Write-Host "Generated $iconPath"

        if ($null -ne $seedPacketImage) {
            $plantIcon = [System.Drawing.Bitmap]::FromFile($iconPath)
            $packetIconPath = Join-Path $packetOutputPath ($plantDir.Name + ".png")
            Export-SeedPacketIcon $plantIcon $seedPacketImage $packetIconPath
            $plantIcon.Dispose()
            Write-Host "Generated $packetIconPath"
        }
    }
}

if ($null -ne $seedPacketImage) {
    $seedPacketImage.Dispose()
}

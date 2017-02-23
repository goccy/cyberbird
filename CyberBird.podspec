Pod::Spec.new do |s|
  s.name         = "CyberBird"
  s.version      = "0.0.1"
  s.platform     = :ios, "6.0"
  s.summary      = "fast/compact geo location database for mobile"
  s.description  = "fast/compact geo location database for mobile"
  s.homepage     = "https://github.com/goccy/cyberbird.git"
  s.license      = { :type => 'MIT' }
  s.author       = { "goccy" => "goccy54@gmail.com" }
  s.source       = { :git => "git@github.com:goccy/cyberbird.git" }
  s.requires_arc = false
  s.source_files = "src/**/*.{h,cc}", "ios/CyberBird/*.{h,m,mm}"
  s.public_header_files = "CyberBird/CyberBirdBridge.h"
end

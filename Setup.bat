rd /s /q "ThirdParty"
mkdir ThirdParty
cd ThirdParty
dir

git clone git@github.com:Biggkahuna98/QuestEngineThirdPartyLibs.git

dir

mkdir Engine

dir

Robocopy "QuestEngineThirdPartyLibs" "Engine" /E /MOVE /NFL /NDL /NJH /NJS /nc /ns /np

cd Engine
rd /s /q ".git"

dir
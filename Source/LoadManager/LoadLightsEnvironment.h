#pragma once

namespace KLoader
{
	struct SLoadedComponentData;
}

int LoadPointLightComponent(KLoader::SLoadedComponentData someData);
int LoadEnvironmentSettings(KLoader::SLoadedComponentData someData);
int LoadDecal(KLoader::SLoadedComponentData someData);


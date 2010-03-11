/**
 * Zillians MMO
 * Copyright (C) 2007-2009 Zillians.com, Inc.
 * For more information see http://www.zillians.com
 *
 * Zillians MMO is the library and runtime for massive multiplayer online game
 * development in utility computing model, which runs as a service for every
 * developer to build their virtual world running on our GPU-assisted machines.
 *
 * This is a close source library intended to be used solely within Zillians.com
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/**
 * @date Sep 8, 2009 sdk - Initial version created.
 */

#include "core-api/Prerequisite.h"
#include "core-api/Buffer.h"
#include "vw/services/naive/DemoGameService.h"

#include <iostream>
#include <string>
#include <limits>
#include <tbb/tick_count.h>

#define BOOST_TEST_MODULE NaiveDemoGameServiceTest
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace zillians;
using namespace std;
using namespace zillians::demo::services::naive;

inline float uniform_random()
{
	return (float)std::rand() / (float)RAND_MAX;
}

BOOST_AUTO_TEST_SUITE( NaiveDemoGameServiceTest )

BOOST_AUTO_TEST_CASE( NaiveDemoGameServiceTestCase1 )
{
	std::srand(95123);
	DemoGameService::GameConfiguration configuration;

	if(1)
	{
		configuration.max_object_count = 131072;
		configuration.max_updates_count_per_object = 64;
		configuration.max_updates_count = configuration.max_object_count * configuration.max_updates_count_per_object;
		configuration.map_info.world_map_max_x = 1000.0f;
		configuration.map_info.world_map_min_x = 0.0f;
		configuration.map_info.world_map_max_y = 1000.0f;
		configuration.map_info.world_map_min_y = 0.0f;
		configuration.map_info.visibility_range = 2.0f;
	}

	if(0)
	{
		configuration.max_object_count = 64;
		configuration.max_updates_count_per_object = 32;
		configuration.max_updates_count = configuration.max_object_count * configuration.max_updates_count_per_object;
		configuration.map_info.world_map_max_x = 10.0f;
		configuration.map_info.world_map_min_x = 0.0f;
		configuration.map_info.world_map_max_y = 10.0f;
		configuration.map_info.world_map_min_y = 0.0f;
		configuration.map_info.visibility_range = 2.0f;
	}

	uint32 max_command_count = configuration.max_object_count/5;

	DemoGameService service(configuration);

	Buffer command_buffer(configuration.max_object_count * (sizeof(uint32) * 16));
	Buffer update_buffer(configuration.max_updates_count * (sizeof(uint32) * 16));

	// load all game objects
	printf("loading game objects...");
	std::vector<uint32> objectIds;
	uint32 defaultGatewayId = 0;
	for(uint32 i=0;i<configuration.max_object_count;++i)
	{
		uint32 objectId = 0;
		service.load(i, defaultGatewayId, objectId);
		objectIds.push_back(objectId);
	}
	printf("done, %ld objects loaded\n", objectIds.size());

	// create command buffer
	printf("building command buffer...");
	uint32 command_count = 0;
	for(std::vector<uint32>::const_iterator it = objectIds.begin(); it < objectIds.end(); ++it)
	{
		uint32 commandId = 0x01; // move command
		uint32 objectId = *it;
		float x = uniform_random() * (configuration.map_info.world_map_max_x - configuration.map_info.world_map_min_x) + configuration.map_info.world_map_min_x;
		float y = uniform_random() * (configuration.map_info.world_map_max_y - configuration.map_info.world_map_min_y) + configuration.map_info.world_map_min_y;
		float vx = 1.0f;
		float vy = 1.0f;

		uint32 padding = 0;
		command_buffer << commandId << objectId << x << y << vx << vy;
		for(uint32 i=0;i<10;++i)
		{
			command_buffer << padding;
		}

		++command_count;
		if(command_count >= max_command_count)
			break;
	}
	printf("done, command_count = %d, command_buffer.dataSize() = %ld\n", command_count, command_buffer.dataSize());

	tbb::tick_count start, stop;

	// dispatch command buffer
	printf("dispatching command buffer...");
	start = tbb::tick_count::now();
	service.dispathCommands(command_buffer);       
	stop = tbb::tick_count::now();
	printf("done, takes %f ms, expected updates = %d, actual updates = %d, avg neighbor count = %f\n", (stop-start).seconds()*1000.0, service.mExpectedUpdateCount, service.mActualUpdateCount, service.mAvgNeighborCount);

	// enumerate and get updates back
	printf("enumerating update buffer...");
	start = tbb::tick_count::now();
	service.enumerateUpdates(update_buffer);
	stop = tbb::tick_count::now();
	printf("done, takes %f ms, update_buffer.dataSize() = %ld\n", (stop-start).seconds()*1000.0, update_buffer.dataSize());

#define UPDATE_BUFFER_SIZE  (sizeof(uint32)*16)

	// check the result
//	uint32 update_count = update_buffer.dataSize() / UPDATE_BUFFER_SIZE;
//
//    update_buffer << target.gatewayId << target.sessionId << owner;
//    update_buffer << sourceId << source.info.x << source.info.y << source.info.vx << source.info.vy;
//	uint32 padding = 0;
//
//	for(uint32 i=0;i<update_count;++i)
//	{
//		
//	}
}

BOOST_AUTO_TEST_SUITE_END()

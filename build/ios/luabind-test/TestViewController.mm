//
//  TestViewController.m
//  luabind
//
//  Created by Hao Wu on 4/17/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import "TestViewController.h"
#include "lua/lua.hpp"
#include <luabind/open.hpp>
#include <luabind/error.hpp>
#include <iostream>

struct lua_state
{
    lua_state();
    ~lua_state();
    
    operator lua_State*() const;
    void check() const;
    
private:
    lua_State* m_state;
    int m_top;
};

typedef void (*TestFunc)(lua_State*);

extern void test_yield_main(lua_State* L);
extern void benchmark_test(lua_State* L);

static TestFunc allTests[] = {
    test_yield_main,
    benchmark_test,
};

@interface TestViewController ()

@end

@implementation TestViewController

@synthesize tableView;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table View Source

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return sizeof(allTests)/sizeof(TestFunc);
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
	static NSString *kCustomCellID = @"CustomCellID";
	
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:kCustomCellID];
	if (cell == nil)
	{
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:kCustomCellID] autorelease];
	}
    
	//NSArray* keys = sampleControllers.allKeys;
	cell.textLabel.text = [NSString stringWithFormat: @"run test %d", indexPath.row];//[keys objectAtIndex: indexPath.row];
	//cell.detailTextLabel.text =
	
	return cell;
}

#pragma mark - Table View Delegate
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    [self.tableView deselectRowAtIndexPath:indexPath animated:YES];

    using namespace luabind;
    
	lua_state L;
	try
	{
		allTests[indexPath.row](L);
		L.check();
	}
	catch (luabind::error const& e)
	{
		std::cerr << "Terminated with exception: \"" << e.what() << "\"\n"
        << lua_tostring(e.state(), -1) << "\n";
	}
	catch (std::exception const& e)
	{
		std::cerr << "Terminated with exception: \"" << e.what() << "\"\n";
	}
	catch (...)
	{
		std::cerr << "Terminated with unknown exception\n";
	}
    
    // TODO: push result view
}

@end

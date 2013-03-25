//
//  ViewController.m
//  SampleBrowser
//
//  Created by Hao Wu on 3/24/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

@synthesize sampleControllers;

- (void) dealloc{
    self.sampleControllers = nil;
    [super dealloc];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    self.sampleControllers = [NSDictionary dictionaryWithObjectsAndKeys: [UINavigationController class], @"Name", nil];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table View Source

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return [sampleControllers count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
	static NSString *kCustomCellID = @"CustomCellID";
	
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:kCustomCellID];
	if (cell == nil)
	{
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:kCustomCellID] autorelease];
	}
    
	NSArray* keys = sampleControllers.allKeys;
	cell.textLabel.text = [keys objectAtIndex: indexPath.row];
	//cell.detailTextLabel.text =
	
	return cell;
}

#pragma mark - Table View Delegate
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    
	NSArray* keys = sampleControllers.allKeys;
	NSString* key = [keys objectAtIndex: indexPath.row];
    Class klz = [sampleControllers objectForKey:key];
    UIViewController* controller = [[[klz alloc] init] autorelease];
    [self presentViewController:controller animated:YES
                     completion:^{
                         nil;
                     }];
}

@end

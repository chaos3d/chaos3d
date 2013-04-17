//
//  TestViewController.h
//  luabind
//
//  Created by Hao Wu on 4/17/13.
//  Copyright (c) 2013 cHaos3D. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TestViewController : UIViewController<UITableViewDataSource, UITableViewDelegate>

@property (retain) IBOutlet UITableView* tableView;
@end

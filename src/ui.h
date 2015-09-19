/*
Stereoscopic tunnel for iOS.
Copyright (C) 2011  John Tsiombikas <nuclear@member.fsf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


/* UI and shit */

#import <UIKit/UIKit.h>

@interface UI : UIViewController {
	IBOutlet UISwitch *sw_stereo;
	IBOutlet UISlider *slider_split;
	IBOutlet UISegmentedControl *grp_mode;
	IBOutlet UIButton *bn_done;
}

@property (nonatomic, retain) IBOutlet UISwitch *sw_stereo;
@property (nonatomic, retain) IBOutlet UISlider *slider_split;
@property (nonatomic, retain) IBOutlet UISegmentedControl *grp_mode;
@property (nonatomic, retain) IBOutlet UIButton *bn_done;

-(IBAction) done_clicked: (id) sender;
-(IBAction) split_changed: (id) sender;
-(IBAction) stereo_changed: (id) sender;
-(IBAction) mode_changed: (id) sender;

@end

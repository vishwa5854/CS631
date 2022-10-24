// (void)printf("%s\n", paths[0]);
    char* const* file_paths = (argc >= MIN_NUM_ARGS) && (argv[argc - 1][0] != '-') ? 
                                                            paths : default_path;
    handle = fts_open(file_paths, FTS_FLAGS, &set_sort_flags_and_call_sort);

    while ((node = fts_read(handle)) != NULL) {
        if (flags.d) {
            (void)printf("Length of the name : %ld\n", strlen(node->fts_name));
            print(&flags, node);
            break;
        }
        bool pre_conditions = (node != NULL) && (node->fts_level > DEFAULT_LEVEL);
        pre_conditions = pre_conditions || (node->fts_info == FTS_DP);
        pre_conditions = pre_conditions || (strlen(node->fts_name) && (strncmp("/", node->fts_name, 1) == 0));

        if (pre_conditions) {
            node = node->fts_link;
            continue;
        }
        size_t length_of_file_name = strlen(node->fts_name);
        bool conditions = !flags.a;
        conditions = conditions && ((strncmp(".", node->fts_name, length_of_file_name) == 0) ||
                                    (strncmp("..", node->fts_name, length_of_file_name) == 0));
        conditions = conditions || (!flags.show_hidden_files &&
                                    (node->fts_name[0] == '.'));

        if (conditions) {
            node = node->fts_link;
            continue;
        }
        
        print(&flags, node);

        if (!flags.R && (node->fts_info == FTS_D)) {
            (void)fts_set(handle, node, FTS_SKIP);
        } else {
            node = node->fts_link;
        }
    }
    (void)fts_close(handle);